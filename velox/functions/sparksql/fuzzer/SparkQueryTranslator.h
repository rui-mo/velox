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

#include "velox/exec/fuzzer/QueryTranslator.h"

namespace facebook::velox::functions::sparksql::fuzzer {

class SparkQueryTranslator : public exec::test::QueryTranslator {
 public:
  bool isSupportedType(const TypePtr& type) override;

 protected:
  std::optional<std::string> toSql(
      const std::shared_ptr<const velox::core::AggregationNode>&
          aggregationNode) override;
};
} // namespace facebook::velox::functions::sparksql::fuzzer
