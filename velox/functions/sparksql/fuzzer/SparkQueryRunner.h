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
#pragma once

#include "grpc++/channel.h"
#include "grpc++/client_context.h"
#include "velox/common/memory/Memory.h"
#include "velox/exec/fuzzer/ReferenceQueryRunner.h"
#include "velox/functions/sparksql/fuzzer/proto/spark/connect/base.grpc.pb.h"
#include "velox/vector/ComplexVector.h"

namespace facebook::velox::functions::sparksql::fuzzer {

class SparkQueryRunner : public velox::exec::test::ReferenceQueryRunner {
 public:
  SparkQueryRunner(const std::string& coordinatorUri);

  /// Converts Velox query plan to Spark SQL. Supports Values -> Aggregation.
  ///
  /// Values node is converted into reading from 'tmp' table.
  ///
  /// @return std::nullopt if Values node uses types not supported by Parquet
  /// file format (DATE, INTERVAL, UNKNOWN).
  std::optional<std::string> toSql(
      const velox::core::PlanNodePtr& plan) override;

  std::multiset<std::vector<velox::variant>> execute(
      const std::string& sql,
      const std::vector<velox::RowVectorPtr>& input,
      const velox::RowTypePtr& resultType) override;

  std::multiset<std::vector<velox::variant>> execute(
      const std::string& sql,
      const std::vector<RowVectorPtr>& probeInput,
      const std::vector<RowVectorPtr>& buildInput,
      const RowTypePtr& resultType) override {
    VELOX_NYI();
  }

  bool supportsVeloxVectorResults() const override {
    return true;
  }

  std::vector<velox::RowVectorPtr> executeVector(
      const std::string& sql,
      const std::vector<velox::RowVectorPtr>& input,
      const velox::RowTypePtr& resultType) override;

  std::vector<velox::RowVectorPtr> execute(const std::string& sql) override;

 private:
  velox::memory::MemoryPool* pool() {
    return pool_.get();
  }

  velox::memory::MemoryPool* rootPool() {
    return rootPool_.get();
  }

  std::optional<std::string> toSql(
      const std::shared_ptr<const velox::core::AggregationNode>&
          aggregationNode);

  std::vector<velox::RowVectorPtr> readArrowData(const std::string& data);

  std::unique_ptr<spark::connect::SparkConnectService::Stub> stub_;
  google::protobuf::Arena arena_;
  inline static const std::string kUserId = "veloxUser";
  inline static const std::string kUserName = "veloxTest";
  // In Spark, Handle must be an UUID string of the format
  // '00112233-4455-6677-8899-aabbccddeeff'.
  inline static const std::string kSessionId =
      "70f50bc3-d60c-4ceb-8828-65de803561d8";
  std::shared_ptr<velox::memory::MemoryPool> rootPool_{
      velox::memory::memoryManager()->addRootPool()};
  std::shared_ptr<velox::memory::MemoryPool> pool_{
      rootPool_->addLeafChild("leaf")};
  std::shared_ptr<velox::memory::MemoryPool> copyPool_{
      rootPool_->addLeafChild("copy")};
};
} // namespace facebook::velox::functions::sparksql::fuzzer
