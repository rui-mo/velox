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

#include "velox/core/Expressions.h"
#include "velox/substrait/SubstraitUtils.h"

namespace facebook::velox::substrait {

struct PlanNodeInfo {
  int32_t id;
  const RowTypePtr& rowType;
};

/// This class is used to convert Substrait representations to Velox
/// expressions.
class SubstraitVeloxExprConverter {
 public:
  /// subParser: A Substrait parser used to convert Substrait representations
  /// into recognizable representations. functionMap: A pre-constructed map
  /// storing the relations between the function id and the function name.
  explicit SubstraitVeloxExprConverter(
      const std::unordered_map<uint64_t, std::string>& functionMap)
      : functionMap_(functionMap) {}

  /// Used to convert Substrait Field into Velox Field Expression.
  std::shared_ptr<const core::FieldAccessTypedExpr> toVeloxExpr(
      const ::substrait::Expression::FieldReference& sField,
      const std::vector<PlanNodeInfo>& inputPlanNodeInfos);

  /// Used to convert Substrait ScalarFunction into Velox Expression.
  std::shared_ptr<const core::ITypedExpr> toVeloxExpr(
      const ::substrait::Expression::ScalarFunction& sFunc,
      const std::vector<PlanNodeInfo>& inputPlanNodeInfos);

  /// Used to convert Substrait Literal into Velox Expression.
  std::shared_ptr<const core::ConstantTypedExpr> toVeloxExpr(
      const ::substrait::Expression::Literal& sLit);

  /// Used to convert Substrait Expression into Velox Expression.
  std::shared_ptr<const core::ITypedExpr> toVeloxExpr(
      const ::substrait::Expression& sExpr,
      const std::vector<PlanNodeInfo>& inputPlanNodes);

 private:
  /// The Substrait parser used to convert Substrait representations into
  /// recognizable representations.
  std::shared_ptr<SubstraitParser> subParser_ =
      std::make_shared<SubstraitParser>();

  /// The map storing the relations between the function id and the function
  /// name.
  std::unordered_map<uint64_t, std::string> functionMap_ = {};
};

} // namespace facebook::velox::substrait
