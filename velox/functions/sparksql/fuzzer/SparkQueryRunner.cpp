/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <memory>
#include <string>

#include "arrow/buffer.h"
#include "arrow/c/bridge.h"
#include "arrow/io/api.h"
#include "arrow/ipc/api.h"
#include "arrow/result.h"
#include "arrow/status.h"
#include "grpc++/create_channel.h"
#include "grpc++/security/credentials.h"
#include "grpc/grpc.h"
#include "velox/common/base/Fs.h"
#include "velox/dwio/common/WriterFactory.h"
#include "velox/exec/fuzzer/proto/spark/connect/base.pb.h"
#include "velox/exec/fuzzer/proto/spark/connect/relations.pb.h"
#include "velox/exec/tests/utils/QueryAssertions.h"
#include "velox/exec/tests/utils/TempFilePath.h"
#include "velox/functions/sparksql/fuzzer/SparkQueryRunner.h"
#include "velox/vector/arrow/Abi.h"
#include "velox/vector/arrow/Bridge.h"

using namespace spark::connect;

namespace facebook::velox::functions::sparksql::fuzzer {
namespace {

void writeToFile(
    const std::string& path,
    const std::vector<RowVectorPtr>& data,
    memory::MemoryPool* pool) {
  VELOX_CHECK_GT(data.size(), 0);

  dwio::common::WriterOptions options;
  options.schema = data[0]->type();
  options.memoryPool = pool;
  // Spark does not recognize timestamp written as nano unit and int64 type in
  // Parquet.
  options.parquetWriteTimestampUnit = 6 /*kMirco*/;

  auto writeFile = std::make_unique<LocalWriteFile>(path, true, false);
  auto sink =
      std::make_unique<dwio::common::WriteFileSink>(std::move(writeFile), path);
  auto writer =
      dwio::common::getWriterFactory(dwio::common::FileFormat::PARQUET)
          ->createWriter(std::move(sink), options);

  for (const auto& vector : data) {
    writer->write(vector);
  }
  writer->close();
}
} // namespace

SparkQueryRunner::SparkQueryRunner(const std::string& coordinatorUri) {
  std::shared_ptr<grpc::Channel> channel =
      grpc::CreateChannel(coordinatorUri, grpc::InsecureChannelCredentials());
  stub_ = spark::connect::SparkConnectService::NewStub(channel);
  queryTranslator_ = std::make_shared<SparkQueryTranslator>();
}

std::vector<RowVectorPtr> SparkQueryRunner::execute(
    const std::string& content) {
  auto sql = google::protobuf::Arena::CreateMessage<SQL>(&arena_);
  sql->set_query(content);

  auto relation = google::protobuf::Arena::CreateMessage<Relation>(&arena_);
  relation->set_allocated_sql(sql);

  auto plan = google::protobuf::Arena::CreateMessage<Plan>(&arena_);
  plan->set_allocated_root(relation);

  auto context = google::protobuf::Arena::CreateMessage<UserContext>(&arena_);
  context->set_user_id(kUserId);
  context->set_user_name(kUserName);

  auto request =
      google::protobuf::Arena::CreateMessage<ExecutePlanRequest>(&arena_);
  request->set_session_id(kSessionId);
  request->set_allocated_user_context(context);
  request->set_allocated_plan(plan);

  grpc::ClientContext clientContext;
  auto reader = stub_->ExecutePlan(&clientContext, *request);
  ExecutePlanResponse response;
  std::vector<RowVectorPtr> results;
  while (reader->Read(&response)) {
    if (response.session_id() == kSessionId && response.has_arrow_batch()) {
      const std::string& data = response.arrow_batch().data();
      const auto batchResults = readArrowData(data);
      results.insert(results.end(), batchResults.begin(), batchResults.end());
    }
  }
  return results;
}

std::multiset<std::vector<variant>> SparkQueryRunner::execute(
    const std::string& sql,
    const std::vector<RowVectorPtr>& input,
    const RowTypePtr& resultType) {
  return exec::test::materialize(executeVector(sql, input, resultType));
}

std::vector<RowVectorPtr> SparkQueryRunner::executeVector(
    const std::string& sql,
    const std::vector<RowVectorPtr>& input,
    const RowTypePtr& resultType) {
  auto inputType = asRowType(input[0]->type());
  if (inputType->size() == 0) {
    // The query doesn't need to read any columns, but it needs to see a
    // specific number of rows. Make new 'input' as single all-null BIGINT
    // column with as many rows as original input. This way we'll be able to
    // create a 'tmp' table will the necessary number of rows.
    vector_size_t numInput = 0;
    for (const auto& v : input) {
      numInput += v->size();
    }

    auto column = BaseVector::createNullConstant(BIGINT(), numInput, pool());
    auto rowVector = std::make_shared<RowVector>(
        pool(),
        ROW({"x"}, {BIGINT()}),
        nullptr,
        numInput,
        std::vector<VectorPtr>{column});
    return executeVector(sql, {rowVector}, resultType);
  }

  // Write the input to a Parquet file.
  auto tempFile = exec::test::TempFilePath::create();
  const auto& filePath = tempFile->getPath();
  auto writerPool = rootPool()->addAggregateChild("writer");
  writeToFile(filePath, input, writerPool.get());

  // Run the query.
  execute(fmt::format(
      "CREATE OR REPLACE TEMPORARY VIEW tmp AS (SELECT * from parquet.`file://{}`);",
      filePath));
  return execute(sql);
}

bool SparkQueryRunner::supportsVeloxVectorResults() const {
  return true;
}

std::optional<std::string> SparkQueryRunner::toSql(
    const velox::core::PlanNodePtr& plan) {
  return queryTranslator_->QueryTranslator::toSql(plan);
}

std::vector<RowVectorPtr> SparkQueryRunner::readArrowData(
    const std::string& data) {
  auto buffer = std::make_shared<arrow::Buffer>(data);
  auto bufferReader = std::make_shared<arrow::io::BufferReader>(buffer);
  arrow::Result<std::shared_ptr<arrow::ipc::RecordBatchReader>> result =
      arrow::ipc::RecordBatchStreamReader::Open(bufferReader);
  VELOX_CHECK(
      result.ok(),
      "Failed to open RecordBatchReader: {}.",
      result.status().ToString());

  std::shared_ptr<arrow::ipc::RecordBatchReader> reader = result.ValueUnsafe();
  std::shared_ptr<arrow::Schema> schema = reader->schema();

  ArrowSchema arrowSchema;
  auto status = arrow::ExportSchema(*schema, &arrowSchema);
  VELOX_CHECK(status.ok(), "Failed to export schema: {}.", status.ToString());

  arrow::Result<std::shared_ptr<arrow::RecordBatch>> batchResult;
  std::vector<RowVectorPtr> results;
  while ((batchResult = reader->Next()).ok() && batchResult.ValueUnsafe()) {
    std::shared_ptr<arrow::RecordBatch> batch = batchResult.ValueUnsafe();
    ArrowArray arrowArray;
    status = ExportRecordBatch(*batch, &arrowArray);
    VELOX_CHECK(status.ok(), "Failed to export array: {}.", status.ToString());
    results.push_back(std::dynamic_pointer_cast<RowVector>(
        importFromArrowAsOwner(arrowSchema, arrowArray, pool())));
  }
  VELOX_CHECK(
      batchResult.ok(),
      "Failed to read batch: {}.",
      batchResult.status().ToString());
  return results;
}
} // namespace facebook::velox::functions::sparksql::fuzzer
