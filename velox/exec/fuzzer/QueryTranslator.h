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

#include "velox/core/PlanNode.h"

namespace facebook::velox::exec::test {

/// Query translator to generate compatible query on a reference database, i.e.
/// DuckDB, Presto, Spark, from Velox plan node.
class QueryTranslator {
 public:
  virtual ~QueryTranslator() = default;

  void appendComma(int32_t i, std::stringstream& sql);

  /// Converts Velox plan into SQL accepted by the reference database.
  /// @return std::nullopt if the plan uses features not supported by the
  /// reference database.
  virtual std::optional<std::string> toSql(const core::PlanNodePtr& plan);

  virtual std::string toTypeSql(const TypePtr& type);

  /// Returns false if the type or its children are unsupported.
  /// Currently returns false if type is Date,IntervalDayTime or Unknown.
  /// @param type
  /// @return bool
  virtual bool isSupportedType(const TypePtr& type);

 protected:
  void appendWindowFrame(
      const core::WindowNode::Frame& frame,
      std::stringstream& sql);

  std::string toAggregateCallSql(
      const core::CallTypedExprPtr& call,
      const std::vector<core::FieldAccessTypedExprPtr>& sortingKeys,
      const std::vector<core::SortOrder>& sortingOrders,
      bool distinct);

  void toCallInputsSql(
      const std::vector<core::TypedExprPtr>& inputs,
      std::stringstream& sql);

  std::string toCallSql(const core::CallTypedExprPtr& call);

  virtual std::optional<std::string> toSql(
      const std::shared_ptr<const velox::core::AggregationNode>&
          aggregationNode);

  std::optional<std::string> toSql(
      const std::shared_ptr<const velox::core::WindowNode>& windowNode);

  std::optional<std::string> toSql(
      const std::shared_ptr<const velox::core::ProjectNode>& projectNode);

  std::optional<std::string> toSql(
      const std::shared_ptr<const velox::core::RowNumberNode>& rowNumberNode);

  std::string toWindowCallSql(
      const core::CallTypedExprPtr& call,
      bool ignoreNulls = false);
};
} // namespace facebook::velox::exec::test
