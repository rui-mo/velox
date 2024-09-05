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
#include "velox/expression/DecimalAvgResultGenerator.h"

namespace facebook::velox::functions::aggregate::sparksql {
namespace {

// Returns all the possible decimal types.
const std::vector<TypePtr>& getAllTypes() {
  const auto generateAllTypes = []() {
    std::vector<TypePtr> allTypes;
    for (auto p = 1; p <= 38; ++p) {
      for (auto s = 0; s <= p; ++s) {
        allTypes.push_back(DECIMAL(p, s));
      }
    }
    return allTypes;
  };

  static const std::vector<TypePtr> allTypes = generateAllTypes();
  return allTypes;
}
} // namespace

TypePtr DecimalAvgResultGenerator::generateResultType(
    const TypePtr& intermediateType) {
  VELOX_CHECK(
      intermediateType->isRow(),
      "The intermediate type of decimal average should be row type.");
  auto decimalType = intermediateType->asRow().children()[0];
  const auto [p, s] = getDecimalPrecisionScale(*decimalType);
  const auto it = types_.find({p, s});
  if (it == types_.end()) {
    VLOG(1) << "Cannot find result for " << intermediateType->toString();
    return nullptr;
  }
  return it->second;
}

void DecimalAvgResultGenerator::initialize() {
  for (const auto& t : getAllTypes()) {
    auto [p, s] = getDecimalPrecisionScale(*t);
    auto precision = std::min(p + 10, 38);
    std::pair<uint8_t, uint8_t> intermediate = {precision, s};
    if (types_.count(intermediate) == 0) {
      if (precision == 38) {
        // When the intermediate precision is 38, the result precision can be in
        // the range of [32, 38].
        auto rScale = std::min(s + 4, 38);
        types_[intermediate] = DECIMAL(38, rScale);
      } else {
        types_[intermediate] = DECIMAL(p + 4, s + 4);
      }
    }
  }
}
} // namespace facebook::velox::functions::aggregate::sparksql
