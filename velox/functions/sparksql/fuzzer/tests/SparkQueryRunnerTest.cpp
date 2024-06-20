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

#include <gtest/gtest.h>

#include "velox/common/base/tests/GTestUtils.h"
#include "velox/exec/tests/utils/AssertQueryBuilder.h"
#include "velox/exec/tests/utils/PlanBuilder.h"
#include "velox/functions/sparksql/Register.h"
#include "velox/functions/sparksql/aggregates/Register.h"
#include "velox/functions/sparksql/fuzzer/SparkQueryRunner.h"
#include "velox/functions/sparksql/window/WindowFunctionsRegistration.h"
#include "velox/parse/TypeResolver.h"
#include "velox/vector/tests/utils/VectorTestBase.h"

using namespace facebook::velox;
using namespace facebook::velox::test;

namespace facebook::velox::functions::sparksql::test {

class SparkQueryRunnerTest : public ::testing::Test,
                             public velox::test::VectorTestBase {
 protected:
  static void SetUpTestCase() {
    memory::MemoryManager::testingSetInstance({});
  }

  void SetUp() override {
    velox::functions::sparksql::registerFunctions("");
    velox::functions::aggregate::sparksql::registerAggregateFunctions("");
    velox::parse::registerTypeResolver();
  }
};

// This test requires a Spark Coordinator running at localhost, so disable it
// by default.
TEST_F(SparkQueryRunnerTest, basic) {
  auto queryRunner =
      std::make_unique<fuzzer::SparkQueryRunner>("localhost:15002");

  auto expected = makeRowVector({
      makeConstant<int64_t>(1010, 1, DECIMAL(10, 2)),
  });
  auto results = queryRunner->execute("SELECT cast(10.1 as decimal(10, 2))");
  exec::test::assertEqualResults({expected}, results);

  auto input = makeRowVector({
      makeConstant<int64_t>(25, 10'000),
  });
  expected = makeRowVector({
      makeConstant<std::string>("25", 10'000),
  });
  results = queryRunner->executeVector(
      "SELECT cast(c0 as string) from tmp", {input}, ROW({"a"}, {VARCHAR()}));
  exec::test::assertEqualResults({expected}, results);
}

} // namespace facebook::velox::functions::sparksql::test
