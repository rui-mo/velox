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

#include "velox/substrait/SubstraitToVeloxPlan.h"

namespace facebook::velox::substrait {

/// This class is used to validate whether the computing of
/// a Substrait plan is supported in Velox.
class SubstraitToVeloxPlanValidator {
 public:
  SubstraitToVeloxPlanValidator() {}

  /// Used to validate literal.
  bool validate(const ::substrait::Expression::Literal& sLit);

  bool validate(const ::substrait::Expression::FieldReference& sField);

  bool validate(const ::substrait::Expression::ScalarFunction& sFunc);

  /// Used to validate expressions.
  bool validate(const ::substrait::Expression& sExpr);

  /// Used to validate Aggregation.
  bool validate(const ::substrait::AggregateRel& sAgg);

  /// Used to validate Project.
  bool validate(const ::substrait::ProjectRel& sProject);

  /// Used to validate Filter.
  bool validate(const ::substrait::FilterRel& sFilter);

  /// Used to validate TableScan.
  bool validate(const ::substrait::ReadRel& sRead);

  /// Used to validate whether the computing of this Rel is supported.
  bool validate(const ::substrait::Rel& sRel);

 private:
};

} // namespace facebook::velox::substrait
