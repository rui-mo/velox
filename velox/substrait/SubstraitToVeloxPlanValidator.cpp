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

bool SubstraitToVeloxPlanValidator::validate(const ::substrait::Type& sType) {
  switch (sType.kind_case()) {
    case ::substrait::Type::KindCase::kBool:
    case ::substrait::Type::KindCase::kI32:
    case ::substrait::Type::KindCase::kI64:
    case ::substrait::Type::KindCase::kFp64:
    case ::substrait::Type::KindCase::kString:
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
  //   if (sFilter.has_input()) {
  //     if (!validate(sFilter.input())) {
  //       return false;
  //     }
  //   }
  return false;
}

bool SubstraitToVeloxPlanValidator::validate(
    const ::substrait::AggregateRel& sAgg) {
  return false;
}

bool SubstraitToVeloxPlanValidator::validate(
    const ::substrait::ReadRel& sRead) {
  if (!sRead.has_base_schema()) {
    std::cout << "Validation failed due to schema was not found in ReadRel."
              << std::endl;
    return false;
  }
  const auto& sTypes = sRead.base_schema().struct_().types();
  for (const auto& sType : sTypes) {
    if (!validate(sType)) {
      std::cout << "Validation failed due to type was not supported in ReadRel."
                << std::endl;
      return false;
    }
  }
  std::vector<::substrait::Expression_ScalarFunction> scalarFunctions;
  if (sRead.has_filter()) {
    try {
      planConverter_->flattenConditions(sRead.filter(), scalarFunctions);
    } catch (const VeloxException& err) {
      std::cout
          << "Validation failed due to flattening conditions failed in ReadRel."
          << std::endl;
      return false;
    }
  }
  // Get and validate the filter functions.
  std::vector<std::string> funcSpecs;
  funcSpecs.reserve(scalarFunctions.size());
  for (const auto& scalarFunction : scalarFunctions) {
    try {
      funcSpecs.emplace_back(
          planConverter_->findFuncSpec(scalarFunction.function_reference()));
    } catch (const VeloxException& err) {
      std::cout << "Validation failed due to function was not found in ReadRel."
                << std::endl;
      return false;
    }
    // Expect there are two args. One is field, and another is literal.
    if (scalarFunction.args().size() == 1) {
      for (const auto& param : scalarFunction.args()) {
        auto typeCase = param.rex_type_case();
        switch (typeCase) {
          case ::substrait::Expression::RexTypeCase::kSelection:
            break;
          default:
            std::cout << "Field is Expected." << std::endl;
            return false;
        }
      }
    } else if (scalarFunction.args().size() == 2) {
      bool fieldExists = false;
      bool litExists = false;
      for (const auto& param : scalarFunction.args()) {
        auto typeCase = param.rex_type_case();
        switch (typeCase) {
          case ::substrait::Expression::RexTypeCase::kSelection: {
            fieldExists = true;
            break;
          }
          case ::substrait::Expression::RexTypeCase::kLiteral: {
            litExists = true;
            break;
          }
          default:
            std::cout << "Type case: " << typeCase
                      << " is not supported in ReadRel." << std::endl;
            return false;
        }
      }
      if (!fieldExists || !litExists) {
        std::cout << "Only the case of Field and Literal is supported."
                  << std::endl;
        return false;
      }
    } else {
      std::cout << "More than two args is not supported in ReadRel."
                << std::endl;
      return false;
    }
  }
  std::unordered_set<std::string> supportedFilters = {
      "is_not_null", "gte", "gt", "lte", "lt"};
  std::unordered_set<std::string> supportedTypes = {"opt", "req", "fp64"};
  for (const auto& funcSpec : funcSpecs) {
    auto funcName = subParser_->getSubFunctionName(funcSpec);
    if (supportedFilters.find(funcName) == supportedFilters.end()) {
      std::cout << "Validation failed due to " << funcName
                << " was not supported in ReadRel." << std::endl;
      return false;
    }
    std::vector<std::string> funcTypes;
    subParser_->getSubFunctionTypes(funcSpec, funcTypes);
    for (const auto& funcType : funcTypes) {
      if (supportedTypes.find(funcType) == supportedTypes.end()) {
        std::cout << "Validation failed due to " << funcType
                  << " was not supported in ReadRel." << std::endl;
        return false;
      }
    }
  }
  return true;
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

bool SubstraitToVeloxPlanValidator::validate(
    const ::substrait::RelRoot& sRoot) {
  if (sRoot.has_input()) {
    const auto& sRel = sRoot.input();
    return validate(sRel);
  }
  return false;
}

bool SubstraitToVeloxPlanValidator::validate(const ::substrait::Plan& sPlan) {
  planConverter_->constructFuncMap(sPlan);
  for (const auto& sRel : sPlan.relations()) {
    if (sRel.has_root()) {
      return validate(sRel.root());
    }
    if (sRel.has_rel()) {
      return validate(sRel.rel());
    }
  }
  return false;
}

} // namespace facebook::velox::substrait
