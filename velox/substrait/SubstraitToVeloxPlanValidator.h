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
  /// Used to validate type.
  bool validate(const ::substrait::Type& sType);

  /// Used to validate Aggregation.
  bool validate(const ::substrait::AggregateRel& sAgg);

  /// Used to validate Project.
  bool validate(const ::substrait::ProjectRel& sProject);

  /// Used to validate Filter.
  bool validate(const ::substrait::FilterRel& sFilter);

  /// Used to validate Join.
  bool validate(const ::substrait::JoinRel& sJoin);
  /// Used to validate TableScan.
  bool validate(const ::substrait::ReadRel& sRead);

  /// Used to validate whether the computing of this Rel is supported.
  bool validate(const ::substrait::Rel& sRel);

  /// Used to validate RelRoot.
  bool validate(const ::substrait::RelRoot& sRoot);

  /// Used to validate whether the computing of this Plan is supported.
  bool validate(const ::substrait::Plan& sPlan);

 private:
  /// A query context used for function validation.
  std::shared_ptr<core::QueryCtx> queryCtx_{core::QueryCtx::createForTest()};

  std::unique_ptr<memory::MemoryPool> pool_{
      memory::getDefaultScopedMemoryPool()};

  /// A execution context used for function validation.
  core::ExecCtx execCtx_{pool_.get(), queryCtx_.get()};

  /// A converter used to convert Substrait plan into Velox's plan node.
  std::shared_ptr<SubstraitVeloxPlanConverter> planConverter_ =
      std::make_shared<SubstraitVeloxPlanConverter>();

  /// A parser used to convert Substrait plan into recognizable representations.
  std::shared_ptr<SubstraitParser> subParser_ =
      std::make_shared<SubstraitParser>();

  /// A Expression converter used to convert Substrait representations into
  /// Velox expressions.
  std::shared_ptr<SubstraitVeloxExprConverter> exprConverter_;

  /// Used to get types from advanced extension and validate them.
  bool validateInputTypes(
      const ::substrait::extensions::AdvancedExtension& extension,
      std::vector<TypePtr>& types);
};

} // namespace facebook::velox::substrait
