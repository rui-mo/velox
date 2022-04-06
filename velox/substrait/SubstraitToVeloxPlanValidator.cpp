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

#include "velox/substrait/SubstraitToVeloxPlanValidator.h"

namespace facebook::velox::substrait {

bool SubstraitToVeloxPlanValidator::validate(
    const ::substrait::Expression::Literal& sLit) {
  auto typeCase = sLit.literal_type_case();
  switch (typeCase) {
    case ::substrait::Expression_Literal::LiteralTypeCase::kI32:
    case ::substrait::Expression_Literal::LiteralTypeCase::kFp64:
    case ::substrait::Expression_Literal::LiteralTypeCase::kBoolean:
      return true;
    default:
      return false;
  }
}

bool SubstraitToVeloxPlanValidator::validate(
    const ::substrait::Expression::FieldReference& sField) {
  return false;
}

bool SubstraitToVeloxPlanValidator::validate(
    const ::substrait::Expression::ScalarFunction& sFunc) {
  return false;
}

bool SubstraitToVeloxPlanValidator::validate(
    const ::substrait::Expression& sExpr) {
  auto typeCase = sExpr.rex_type_case();
  switch (typeCase) {
    case ::substrait::Expression::RexTypeCase::kLiteral:
      return validate(sExpr.literal());
    case ::substrait::Expression::RexTypeCase::kScalarFunction:
      return validate(sExpr.scalar_function());
    case ::substrait::Expression::RexTypeCase::kSelection:
      return validate(sExpr.selection());
    default:
      return false;
  }
}

bool SubstraitToVeloxPlanValidator::validate(
    const ::substrait::ProjectRel& sProject) {
  auto projectExprs = sProject.expressions();
  return false;
}

bool SubstraitToVeloxPlanValidator::validate(
    const ::substrait::FilterRel& sFilter) {
  if (sFilter.has_input()) {
    if (!validate(sFilter.input())) {
      return false;
    }
  }
  return false;
}

bool validate(const ::substrait::AggregateRel& sAgg) {
  return false;
}

bool validate(const ::substrait::ReadRel& sRead) {
  return false;
}

bool SubstraitToVeloxPlanValidator::validate(const ::substrait::Rel& sRel) {
  if (sRel.has_aggregate()) {
    return validate(sRel.aggregate());
  }
  if (sRel.has_project()) {
    return validate(sRel.project());
  }
  if (sRel.has_filter()) {
    return validate(sRel.filter());
  }
  if (sRel.has_read()) {
    return validate(sRel.read());
  }
  return false;
}

} // namespace facebook::velox::substrait
