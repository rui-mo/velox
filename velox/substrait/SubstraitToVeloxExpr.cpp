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

#include "velox/substrait/SubstraitToVeloxExpr.h"
#include "velox/substrait/TypeUtils.h"

namespace facebook::velox::substrait {

std::shared_ptr<const core::FieldAccessTypedExpr>
SubstraitVeloxExprConverter::toVeloxExpr(
    const ::substrait::Expression::FieldReference& sField,
    int32_t inputPlanNodeId, const RowTypePtr& inputType) {
  auto typeCase = sField.reference_type_case();
  switch (typeCase) {
    case ::substrait::Expression::FieldReference::ReferenceTypeCase::
        kDirectReference: {
      auto dRef = sField.direct_reference();
      int32_t colIdx = subParser_->parseReferenceSegment(dRef);
      auto fieldName = subParser_->makeNodeName(inputPlanNodeId, colIdx);
      auto inType = inputType->childAt(colIdx);
      return std::make_shared<const core::FieldAccessTypedExpr>(
          inType, fieldName);
    }
    default:
      VELOX_NYI(
          "Substrait conversion not supported for Reference '{}'", typeCase);
  }
}

std::shared_ptr<const core::ITypedExpr>
SubstraitVeloxExprConverter::toVeloxExpr(
    const ::substrait::Expression::ScalarFunction& sFunc,
    int32_t inputPlanNodeId,
    const RowTypePtr& inputType) {
  std::vector<std::shared_ptr<const core::ITypedExpr>> params;
  params.reserve(sFunc.args().size());
  for (const auto& sArg : sFunc.args()) {
    params.emplace_back(toVeloxExpr(sArg, inputPlanNodeId, inputType));
  }
  auto veloxFunction = subParser_->findVeloxFunction(
    functionMap_, sFunc.function_reference());
  auto veloxType = toVeloxType(
      subParser_->parseType(sFunc.output_type())->type);
  if (veloxFunction == "cast") {
    return std::make_shared<const core::CastTypedExpr>(
      veloxType, std::move(params), true);
  }
  if (veloxFunction == "alias") {
    if (params.size() != 1) {
      VELOX_FAIL("Alias expects one parameter.");
    }
    return params[0];
  }
  return std::make_shared<const core::CallTypedExpr>(
      veloxType, std::move(params), veloxFunction);
}

std::shared_ptr<const core::ConstantTypedExpr>
SubstraitVeloxExprConverter::toVeloxExpr(
    const ::substrait::Expression::Literal& sLit) {
  auto typeCase = sLit.literal_type_case();
  switch (typeCase) {
    case ::substrait::Expression_Literal::LiteralTypeCase::kI32:
      return std::make_shared<core::ConstantTypedExpr>(sLit.i32());
    case ::substrait::Expression_Literal::LiteralTypeCase::kFp64:
      return std::make_shared<core::ConstantTypedExpr>(sLit.fp64());
    case ::substrait::Expression_Literal::LiteralTypeCase::kBoolean:
      return std::make_shared<core::ConstantTypedExpr>(sLit.boolean());
    default:
      VELOX_NYI(
          "Substrait conversion not supported for type case '{}'", typeCase);
  }
}

std::shared_ptr<const core::ITypedExpr>
SubstraitVeloxExprConverter::toVeloxExpr(
    const ::substrait::Expression& sExpr,
    int32_t inputPlanNodeId, const RowTypePtr& inputType) {
  std::shared_ptr<const core::ITypedExpr> veloxExpr;
  auto typeCase = sExpr.rex_type_case();
  switch (typeCase) {
    case ::substrait::Expression::RexTypeCase::kLiteral:
      return toVeloxExpr(sExpr.literal());
    case ::substrait::Expression::RexTypeCase::kScalarFunction:
      return toVeloxExpr(sExpr.scalar_function(), inputPlanNodeId, inputType);
    case ::substrait::Expression::RexTypeCase::kSelection:
      return toVeloxExpr(sExpr.selection(), inputPlanNodeId, inputType);
    default:
      VELOX_NYI(
          "Substrait conversion not supported for Expression '{}'", typeCase);
  }
}

} // namespace facebook::velox::substrait
