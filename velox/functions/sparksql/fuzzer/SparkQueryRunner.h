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
#include "velox/exec/fuzzer/proto/spark/connect/base.grpc.pb.h"
#include "velox/functions/sparksql/fuzzer/SparkQueryTranslator.h"
#include "velox/vector/ComplexVector.h"

namespace facebook::velox::functions::sparksql::fuzzer {

class SparkQueryRunner : public velox::exec::test::ReferenceQueryRunner {
 public:
  SparkQueryRunner(const std::string& coordinatorUri);

  std::vector<velox::RowVectorPtr> execute(const std::string& sql);

  std::multiset<std::vector<velox::variant>> execute(
      const std::string& sql,
      const std::vector<velox::RowVectorPtr>& input,
      const velox::RowTypePtr& resultType) override;

  std::vector<velox::RowVectorPtr> executeVector(
      const std::string& sql,
      const std::vector<velox::RowVectorPtr>& input,
      const velox::RowTypePtr& resultType);

  bool supportsVeloxVectorResults() const override;

  std::optional<std::string> toSql(
      const velox::core::PlanNodePtr& plan) override;

 private:
  velox::memory::MemoryPool* pool() {
    return pool_.get();
  }

  velox::memory::MemoryPool* rootPool() {
    return rootPool_.get();
  }

  std::vector<velox::RowVectorPtr> readArrowData(const std::string& data);

  std::unique_ptr<spark::connect::SparkConnectService::Stub> stub_;
  google::protobuf::Arena arena_;
  inline static const std::string kUserId = "u0";
  inline static const std::string kUserName = "native_client";
  inline static const std::string kSessionId =
      "70f50bc3-d60c-4ceb-8828-65de803561d8";
  std::shared_ptr<velox::memory::MemoryPool> rootPool_{
      velox::memory::memoryManager()->addRootPool()};
  std::shared_ptr<velox::memory::MemoryPool> pool_{
      rootPool_->addLeafChild("leaf")};
  std::shared_ptr<SparkQueryTranslator> queryTranslator_;
};
} // namespace facebook::velox::functions::sparksql::fuzzer
