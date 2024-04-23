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

#include "velox/exec/fuzzer/PrestoQueryRunner.h"
#include <cpr/cpr.h> // @manual
#include <folly/json.h>
#include <iostream>
#include "velox/common/base/Fs.h"
#include "velox/common/encode/Base64.h"
#include "velox/common/file/FileSystems.h"
#include "velox/core/Expressions.h"
#include "velox/dwio/common/WriterFactory.h"
#include "velox/exec/tests/utils/QueryAssertions.h"
#include "velox/serializers/PrestoSerializer.h"
#include "velox/type/parser/TypeParser.h"

#include <utility>

using namespace facebook::velox;

namespace facebook::velox::exec::test {

namespace {

template <typename T>
T extractSingleValue(const std::vector<RowVectorPtr>& data) {
  VELOX_CHECK_EQ(1, data.size());
  VELOX_CHECK_EQ(1, data[0]->childrenSize());

  auto simpleVector = data[0]->childAt(0)->as<SimpleVector<T>>();
  VELOX_CHECK(!simpleVector->isNullAt(0));
  return simpleVector->valueAt(0);
}

void writeToFile(
    const std::string& path,
    const std::vector<RowVectorPtr>& data,
    memory::MemoryPool* pool) {
  VELOX_CHECK_GT(data.size(), 0);

  dwio::common::WriterOptions options;
  options.schema = data[0]->type();
  options.memoryPool = pool;

  auto writeFile = std::make_unique<LocalWriteFile>(path, true, false);
  auto sink =
      std::make_unique<dwio::common::WriteFileSink>(std::move(writeFile), path);
  auto writer = dwio::common::getWriterFactory(dwio::common::FileFormat::DWRF)
                    ->createWriter(std::move(sink), options);
  for (const auto& vector : data) {
    writer->write(vector);
  }
  writer->close();
}

ByteInputStream toByteStream(const std::string& input) {
  std::vector<ByteRange> ranges;
  ranges.push_back(
      {reinterpret_cast<uint8_t*>(const_cast<char*>(input.data())),
       (int32_t)input.length(),
       0});
  return ByteInputStream(std::move(ranges));
}

RowVectorPtr deserialize(
    const RowTypePtr& rowType,
    const std::string& input,
    memory::MemoryPool* pool) {
  auto byteStream = toByteStream(input);
  auto serde = std::make_unique<serializer::presto::PrestoVectorSerde>();
  RowVectorPtr result;
  serde->deserialize(&byteStream, pool, rowType, &result, nullptr);
  return result;
}

class ServerResponse {
 public:
  explicit ServerResponse(const std::string& responseJson)
      : response_(folly::parseJson(responseJson)) {}

  void throwIfFailed() const {
    if (response_.count("error") == 0) {
      return;
    }

    const auto& error = response_["error"];

    VELOX_FAIL(
        "Presto query failed: {} {}",
        error["errorCode"].asInt(),
        error["message"].asString());
  }

  std::string queryId() const {
    return response_["id"].asString();
  }

  bool queryCompleted() const {
    return response_.count("nextUri") == 0;
  }

  std::string nextUri() const {
    return response_["nextUri"].asString();
  }

  std::vector<RowVectorPtr> queryResults(memory::MemoryPool* pool) const {
    if (!response_.count("binaryData")) {
      return {};
    }

    std::vector<std::string> names;
    std::vector<TypePtr> types;
    for (const auto& column : response_["columns"]) {
      names.push_back(column["name"].asString());
      types.push_back(parseType(column["type"].asString()));
    }

    auto rowType = ROW(std::move(names), std::move(types));

    // There should be a single column with possibly multiple rows. Each row
    // contains base64-encoded PrestoPage of results.

    std::vector<RowVectorPtr> vectors;
    for (auto& encodedData : response_["binaryData"]) {
      const std::string data =
          encoding::Base64::decode(encodedData.stringPiece());
      vectors.push_back(deserialize(rowType, data, pool));
    }
    return vectors;
  }

 private:
  folly::dynamic response_;
};
} // namespace

PrestoQueryRunner::PrestoQueryRunner(
    std::string coordinatorUri,
    std::string user,
    std::chrono::milliseconds timeout)
    : coordinatorUri_{std::move(coordinatorUri)},
      user_{std::move(user)},
      timeout_(timeout) {
  eventBaseThread_.start("PrestoQueryRunner");
  queryTranslator_ = std::make_shared<QueryTranslator>();
}

std::optional<std::string> PrestoQueryRunner::toSql(
    const velox::core::PlanNodePtr& plan) {
  return queryTranslator_->toSql(plan);
}

std::multiset<std::vector<variant>> PrestoQueryRunner::execute(
    const std::string& sql,
    const std::vector<RowVectorPtr>& input,
    const RowTypePtr& resultType) {
  return exec::test::materialize(executeVector(sql, input, resultType));
}

std::vector<velox::RowVectorPtr> PrestoQueryRunner::executeVector(
    const std::string& sql,
    const std::vector<velox::RowVectorPtr>& input,
    const velox::RowTypePtr& resultType) {
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

  // Create tmp table in Presto using DWRF file format and add a single
  // all-null row to it.

  std::stringstream nullValues;
  for (auto i = 0; i < inputType->size(); ++i) {
    queryTranslator_->appendComma(i, nullValues);
    nullValues << fmt::format(
        "cast(null as {})", queryTranslator_->toTypeSql(inputType->childAt(i)));
  }

  execute("DROP TABLE IF EXISTS tmp");

  execute(fmt::format(
      "CREATE TABLE tmp({}) WITH (format = 'DWRF') AS SELECT {}",
      folly::join(", ", inputType->names()),
      nullValues.str()));

  // Query Presto to find out table's location on disk.
  auto results = execute("SELECT \"$path\" FROM tmp");

  auto filePath = extractSingleValue<StringView>(results);
  auto tableDirectoryPath = fs::path(filePath).parent_path();

  // Delete the all-null row.
  execute("DELETE FROM tmp");

  // Create a new file in table's directory with fuzzer-generated data.
  auto newFilePath = fs::path(tableDirectoryPath)
                         .append("fuzzer.dwrf")
                         .string()
                         .substr(strlen("file:"));

  auto writerPool = rootPool()->addAggregateChild("writer");
  writeToFile(newFilePath, input, writerPool.get());

  // Run the query.
  return execute(sql);
}

std::vector<RowVectorPtr> PrestoQueryRunner::execute(const std::string& sql) {
  auto response = ServerResponse(startQuery(sql));
  response.throwIfFailed();

  vector_size_t numResults = 0;
  std::vector<RowVectorPtr> queryResults;
  for (;;) {
    for (auto& result : response.queryResults(pool_.get())) {
      queryResults.push_back(result);
      numResults += result->size();
    }

    if (response.queryCompleted()) {
      break;
    }

    response = ServerResponse(fetchNext(response.nextUri()));
    response.throwIfFailed();
  }

  return queryResults;
}

std::string PrestoQueryRunner::startQuery(const std::string& sql) {
  auto uri = fmt::format("{}/v1/statement?binaryResults=true", coordinatorUri_);
  cpr::Url url{uri};
  cpr::Body body{sql};
  cpr::Header header({
      {"X-Presto-User", user_},
      {"X-Presto-Catalog", "hive"},
      {"X-Presto-Schema", "tpch"},
      {"Content-Type", "text/plain"},
  });
  cpr::Timeout timeout{timeout_};
  cpr::Response response = cpr::Post(url, body, header, timeout);
  VELOX_CHECK_EQ(
      response.status_code,
      200,
      "POST to {} failed: {}",
      uri,
      response.error.message);
  return response.text;
}

std::string PrestoQueryRunner::fetchNext(const std::string& nextUri) {
  cpr::Url url(nextUri);
  cpr::Header header({{"X-Presto-Client-Binary-Results", "true"}});
  cpr::Timeout timeout{timeout_};
  cpr::Response response = cpr::Get(url, header, timeout);
  VELOX_CHECK_EQ(
      response.status_code,
      200,
      "GET from {} failed: {}",
      nextUri,
      response.error.message);
  return response.text;
}

bool PrestoQueryRunner::supportsVeloxVectorResults() const {
  return true;
}

} // namespace facebook::velox::exec::test
