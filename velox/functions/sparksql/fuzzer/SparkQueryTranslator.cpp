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
#include "velox/functions/sparksql/fuzzer/SparkQueryTranslator.h"

namespace facebook::velox::functions::sparksql::fuzzer {

bool SparkQueryTranslator::isSupportedType(const TypePtr& type) {
  if (type->isDate() || type->isIntervalDayTime() || type->isUnKnown() ||
      type->kind() == TypeKind::VARBINARY || type->kind() == TypeKind::MAP ||
      type->kind() == TypeKind::ARRAY) {
    return false;
  }

  for (auto i = 0; i < type->size(); ++i) {
    const auto& child = type->childAt(i);
    if (child->kind() == TypeKind::ROW || !isSupportedType(child)) {
      return false;
    }
  }

  return true;
}

std::optional<std::string> SparkQueryTranslator::toSql(
    const std::shared_ptr<const velox::core::AggregationNode>&
        aggregationNode) {
  const auto& aggregates = aggregationNode->aggregates();
  for (const auto& aggregate : aggregates) {
    if (!aggregate.sortingKeys.empty()) {
      return std::nullopt;
    }
  }
  return QueryTranslator::toSql(aggregationNode);
}

} // namespace facebook::velox::functions::sparksql::fuzzer
