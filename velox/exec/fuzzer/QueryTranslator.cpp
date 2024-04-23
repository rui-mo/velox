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
#include "velox/exec/fuzzer/QueryTranslator.h"

namespace facebook::velox::exec::test {

std::optional<std::string> QueryTranslator::toSql(
    const core::PlanNodePtr& plan) {
  if (const auto projectNode =
          std::dynamic_pointer_cast<const core::ProjectNode>(plan)) {
    return toSql(projectNode);
  }

  if (const auto windowNode =
          std::dynamic_pointer_cast<const core::WindowNode>(plan)) {
    return toSql(windowNode);
  }

  if (const auto aggregationNode =
          std::dynamic_pointer_cast<const core::AggregationNode>(plan)) {
    return toSql(aggregationNode);
  }

  if (const auto rowNumberNode =
          std::dynamic_pointer_cast<const core::RowNumberNode>(plan)) {
    return toSql(rowNumberNode);
  }

  VELOX_NYI();
}

void QueryTranslator::appendComma(int32_t i, std::stringstream& sql) {
  if (i > 0) {
    sql << ", ";
  }
}

void QueryTranslator::appendWindowFrame(
    const core::WindowNode::Frame& frame,
    std::stringstream& sql) {
  // TODO: Add support for k Range Frames by retrieving the original range bound
  // from WindowNode.
  switch (frame.type) {
    case core::WindowNode::WindowType::kRange:
      sql << " RANGE";
      break;
    case core::WindowNode::WindowType::kRows:
      sql << " ROWS";
      break;
    default:
      VELOX_UNREACHABLE();
  }
  sql << " BETWEEN";

  auto appendBound = [&sql](
                         const core::WindowNode::BoundType& bound,
                         const core::TypedExprPtr& value) {
    switch (bound) {
      case core::WindowNode::BoundType::kUnboundedPreceding:
        sql << " UNBOUNDED PRECEDING";
        break;
      case core::WindowNode::BoundType::kUnboundedFollowing:
        sql << " UNBOUNDED FOLLOWING";
        break;
      case core::WindowNode::BoundType::kCurrentRow:
        sql << " CURRENT ROW";
        break;
      case core::WindowNode::BoundType::kPreceding:
        sql << " " << value->toString() << " PRECEDING";
        break;
      case core::WindowNode::BoundType::kFollowing:
        sql << " " << value->toString() << " FOLLOWING";
        break;
      default:
        VELOX_UNREACHABLE();
    }
  };

  appendBound(frame.startType, frame.startValue);
  sql << " AND";
  appendBound(frame.endType, frame.endValue);
}

bool QueryTranslator::isSupportedType(const TypePtr& type) {
  if (type->isDate() || type->isIntervalDayTime() || type->isUnKnown()) {
    return false;
  }

  for (auto i = 0; i < type->size(); ++i) {
    if (!isSupportedType(type->childAt(i))) {
      return false;
    }
  }

  return true;
}

std::string QueryTranslator::toAggregateCallSql(
    const core::CallTypedExprPtr& call,
    const std::vector<core::FieldAccessTypedExprPtr>& sortingKeys,
    const std::vector<core::SortOrder>& sortingOrders,
    bool distinct) {
  VELOX_CHECK_EQ(sortingKeys.size(), sortingOrders.size());
  std::stringstream sql;
  sql << call->name() << "(";

  if (distinct) {
    sql << "distinct ";
  }

  toCallInputsSql(call->inputs(), sql);

  if (!sortingKeys.empty()) {
    sql << " ORDER BY ";

    for (int i = 0; i < sortingKeys.size(); i++) {
      appendComma(i, sql);
      sql << sortingKeys[i]->name() << " " << sortingOrders[i].toString();
    }
  }

  sql << ")";
  return sql.str();
}

void QueryTranslator::toCallInputsSql(
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

std::string QueryTranslator::toCallSql(const core::CallTypedExprPtr& call) {
  std::stringstream sql;
  sql << call->name() << "(";
  toCallInputsSql(call->inputs(), sql);
  sql << ")";
  return sql.str();
}

std::optional<std::string> QueryTranslator::toSql(
    const std::shared_ptr<const velox::core::AggregationNode>&
        aggregationNode) {
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
      sql << toAggregateCallSql(
          aggregate.call,
          aggregate.sortingKeys,
          aggregate.sortingOrders,
          aggregate.distinct);

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

std::optional<std::string> QueryTranslator::toSql(
    const std::shared_ptr<const velox::core::WindowNode>& windowNode) {
  if (!isSupportedType(windowNode->sources()[0]->outputType())) {
    return std::nullopt;
  }

  std::stringstream sql;
  sql << "SELECT ";

  const auto& inputType = windowNode->sources()[0]->outputType();
  for (auto i = 0; i < inputType->size(); ++i) {
    appendComma(i, sql);
    sql << inputType->nameOf(i);
  }

  sql << ", ";

  const auto& functions = windowNode->windowFunctions();
  for (auto i = 0; i < functions.size(); ++i) {
    appendComma(i, sql);
    sql << toWindowCallSql(functions[i].functionCall, functions[i].ignoreNulls);

    sql << " OVER (";

    const auto& partitionKeys = windowNode->partitionKeys();
    if (!partitionKeys.empty()) {
      sql << "PARTITION BY ";
      for (auto j = 0; j < partitionKeys.size(); ++j) {
        appendComma(j, sql);
        sql << partitionKeys[j]->name();
      }
    }

    const auto& sortingKeys = windowNode->sortingKeys();
    const auto& sortingOrders = windowNode->sortingOrders();

    if (!sortingKeys.empty()) {
      sql << " ORDER BY ";
      for (auto j = 0; j < sortingKeys.size(); ++j) {
        appendComma(j, sql);
        sql << sortingKeys[j]->name() << " " << sortingOrders[j].toString();
      }
    }

    appendWindowFrame(functions[i].frame, sql);
    sql << ")";
  }

  sql << " FROM tmp";

  return sql.str();
}

std::optional<std::string> QueryTranslator::toSql(
    const std::shared_ptr<const velox::core::ProjectNode>& projectNode) {
  auto sourceSql = toSql(projectNode->sources()[0]);
  if (!sourceSql.has_value()) {
    return std::nullopt;
  }

  std::stringstream sql;
  sql << "SELECT ";

  for (auto i = 0; i < projectNode->names().size(); ++i) {
    appendComma(i, sql);
    auto projection = projectNode->projections()[i];
    if (auto field =
            std::dynamic_pointer_cast<const core::FieldAccessTypedExpr>(
                projection)) {
      sql << field->name();
    } else if (
        auto call =
            std::dynamic_pointer_cast<const core::CallTypedExpr>(projection)) {
      sql << toCallSql(call);
    } else {
      VELOX_NYI();
    }

    sql << " as " << projectNode->names()[i];
  }

  sql << " FROM (" << sourceSql.value() << ")";
  return sql.str();
}

std::optional<std::string> QueryTranslator::toSql(
    const std::shared_ptr<const core::RowNumberNode>& rowNumberNode) {
  if (!isSupportedType(rowNumberNode->sources()[0]->outputType())) {
    return std::nullopt;
  }

  std::stringstream sql;
  sql << "SELECT ";

  const auto& inputType = rowNumberNode->sources()[0]->outputType();
  for (auto i = 0; i < inputType->size(); ++i) {
    appendComma(i, sql);
    sql << inputType->nameOf(i);
  }

  sql << ", row_number() OVER (";

  const auto& partitionKeys = rowNumberNode->partitionKeys();
  if (!partitionKeys.empty()) {
    sql << "partition by ";
    for (auto i = 0; i < partitionKeys.size(); ++i) {
      appendComma(i, sql);
      sql << partitionKeys[i]->name();
    }
  }

  sql << ") as row_number FROM tmp";

  return sql.str();
}

std::string QueryTranslator::toTypeSql(const TypePtr& type) {
  switch (type->kind()) {
    case TypeKind::ARRAY:
      return fmt::format("array({})", toTypeSql(type->childAt(0)));
    case TypeKind::MAP:
      return fmt::format(
          "map({}, {})",
          toTypeSql(type->childAt(0)),
          toTypeSql(type->childAt(1)));
    case TypeKind::ROW: {
      const auto& rowType = type->asRow();
      std::stringstream sql;
      sql << "row(";
      for (auto i = 0; i < type->size(); ++i) {
        appendComma(i, sql);
        sql << rowType.nameOf(i) << " ";
        sql << toTypeSql(type->childAt(i));
      }
      sql << ")";
      return sql.str();
    }
    default:
      if (type->isPrimitiveType()) {
        return type->toString();
      }
      VELOX_UNSUPPORTED("Type is not supported: {}", type->toString());
  }
}

std::string QueryTranslator::toWindowCallSql(
    const core::CallTypedExprPtr& call,
    bool ignoreNulls) {
  std::stringstream sql;
  sql << call->name() << "(";
  toCallInputsSql(call->inputs(), sql);
  sql << ")";
  if (ignoreNulls) {
    sql << " IGNORE NULLS";
  }
  return sql.str();
}

} // namespace facebook::velox::exec::test
