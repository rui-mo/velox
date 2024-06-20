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
#include "velox/exec/tests/utils/QueryAssertions.h"
#include "velox/exec/tests/utils/TempFilePath.h"
#include "velox/functions/sparksql/fuzzer/SparkQueryRunner.h"
#include "velox/functions/sparksql/fuzzer/proto/spark/connect/base.pb.h"
#include "velox/functions/sparksql/fuzzer/proto/spark/connect/relations.pb.h"
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
    VectorPtr flattened = vector;
    BaseVector::flattenVector(flattened);
    writer->write(flattened);
  }
  writer->close();
}

bool isSupportedType(const TypePtr& type) {
  if (type->isDate() || type->isIntervalDayTime() || type->isUnKnown()) {
    return false;
  }

  for (auto i = 0; i < type->size(); ++i) {
    const auto& child = type->childAt(i);
    if (!isSupportedType(child)) {
      return false;
    }
  }

  return true;
}

void appendComma(int32_t i, std::stringstream& sql) {
  if (i > 0) {
    sql << ", ";
  }
}

std::string toCallSql(const core::CallTypedExprPtr& call);

void toCallInputsSql(
    const std::vector<core::TypedExprPtr>& inputs,
    std::stringstream& sql) {
  for (auto i = 0; i < inputs.size(); ++i) {
    appendComma(i, sql);

    const auto& input = inputs.at(i);
    if (auto field =
            std::dynamic_pointer_cast<const core::FieldAccessTypedExpr>(
                input)) {
      sql << field->name();
    } else if (
        auto call =
            std::dynamic_pointer_cast<const core::CallTypedExpr>(input)) {
      sql << toCallSql(call);
    } else if (
        auto lambda =
            std::dynamic_pointer_cast<const core::LambdaTypedExpr>(input)) {
      const auto& signature = lambda->signature();
      const auto& body =
          std::dynamic_pointer_cast<const core::CallTypedExpr>(lambda->body());
      VELOX_CHECK_NOT_NULL(body);

      sql << "(";
      for (auto j = 0; j < signature->size(); ++j) {
        appendComma(j, sql);
        sql << signature->nameOf(j);
      }

      sql << ") -> " << toCallSql(body);
    } else {
      VELOX_NYI();
    }
  }
}

std::string toCallSql(const core::CallTypedExprPtr& call) {
  std::stringstream sql;
  sql << call->name() << "(";
  toCallInputsSql(call->inputs(), sql);
  sql << ")";
  return sql.str();
}

std::string toAggregateCallSql(
    const core::CallTypedExprPtr& call,
    bool distinct) {
  std::stringstream sql;
  sql << call->name() << "(";

  if (distinct) {
    sql << "distinct ";
  }

  toCallInputsSql(call->inputs(), sql);

  sql << ")";
  return sql.str();
}
} // namespace

SparkQueryRunner::SparkQueryRunner(const std::string& coordinatorUri) {
  std::shared_ptr<grpc::Channel> channel =
      grpc::CreateChannel(coordinatorUri, grpc::InsecureChannelCredentials());
  stub_ = spark::connect::SparkConnectService::NewStub(channel);
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
  // const auto filePath = "/tmp/test.parquet";
  auto writerPool = rootPool()->addAggregateChild("writer");
  writeToFile(filePath, input, writerPool.get());

  // Run the query.
  execute(fmt::format(
      "CREATE OR REPLACE TEMPORARY VIEW tmp AS (SELECT * from parquet.`file://{}`);",
      filePath));
  return execute(sql);
}

std::optional<std::string> SparkQueryRunner::toSql(
    const velox::core::PlanNodePtr& plan) {
  if (const auto aggregationNode =
          std::dynamic_pointer_cast<const core::AggregationNode>(plan)) {
    return toSql(aggregationNode);
  }
  VELOX_NYI();
}

std::optional<std::string> SparkQueryRunner::toSql(
    const std::shared_ptr<const core::AggregationNode>& aggregationNode) {
  for (const auto& aggregate : aggregationNode->aggregates()) {
    if (!aggregate.sortingKeys.empty()) {
      return std::nullopt;
    }
    const auto functionName = aggregate.call->name();
    if (functionName == "max_by" || functionName == "min_by") {
      VELOX_CHECK_EQ(aggregate.rawInputTypes.size(), 2);
      if (aggregate.rawInputTypes[1]->kind() == TypeKind::MAP) {
        // Spark's `max_by` and `min_by` do not support ordering on map type.
        return std::nullopt;
      }
    }
    if (aggregate.distinct) {
      for (const auto& inputType : aggregate.rawInputTypes) {
        // In Spark, grouping/join/window partition keys cannot be map type.
        if (inputType->kind() == TypeKind::MAP) {
          return std::nullopt;
        }
      }
    }
  }
  for (const auto& key : aggregationNode->groupingKeys()) {
    // In Spark, grouping/join/window partition keys cannot be map type.
    if (key->type()->kind() == TypeKind::MAP) {
      return std::nullopt;
    }
  }

  // Assume plan is Aggregation over Values.
  VELOX_CHECK(aggregationNode->step() == core::AggregationNode::Step::kSingle);

  if (!isSupportedType(aggregationNode->sources()[0]->outputType())) {
    return std::nullopt;
  }

  std::vector<std::string> groupingKeys;
  for (const auto& key : aggregationNode->groupingKeys()) {
    groupingKeys.push_back(key->name());
  }

  std::stringstream sql;
  sql << "SELECT " << folly::join(", ", groupingKeys);

  const auto& aggregates = aggregationNode->aggregates();
  if (!aggregates.empty()) {
    if (!groupingKeys.empty()) {
      sql << ", ";
    }

    for (auto i = 0; i < aggregates.size(); ++i) {
      appendComma(i, sql);
      const auto& aggregate = aggregates[i];
      sql << toAggregateCallSql(aggregate.call, aggregate.distinct);

      if (aggregate.mask != nullptr) {
        sql << " filter (where " << aggregate.mask->name() << ")";
      }
      sql << " as " << aggregationNode->aggregateNames()[i];
    }
  }

  sql << " FROM tmp";

  if (!groupingKeys.empty()) {
    sql << " GROUP BY " << folly::join(", ", groupingKeys);
  }

  return sql.str();
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
    auto rv = std::dynamic_pointer_cast<RowVector>(
        importFromArrowAsOwner(arrowSchema, arrowArray, pool()));
    auto copy =
        BaseVector::create<RowVector>(rv->type(), rv->size(), copyPool_.get());
    copy->copy(rv.get(), 0, 0, rv->size());
    results.push_back(copy);
  }
  VELOX_CHECK(
      batchResult.ok(),
      "Failed to read batch: {}.",
      batchResult.status().ToString());
  return results;
}
} // namespace facebook::velox::functions::sparksql::fuzzer
