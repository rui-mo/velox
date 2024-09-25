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

#include <string>
#include "velox/type/Type.h"

namespace facebook::velox {

TypePtr typeFromString(
    const std::string& type,
    bool failIfNotRegistered = true) {
  auto upper = type;
  std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
  if (upper == "INT") {
    upper = "INTEGER";
  } else if (upper == "DOUBLE PRECISION") {
    upper = "DOUBLE";
  }
  auto inferredType = getType(upper, {});
  if (failIfNotRegistered) {
    VELOX_CHECK(
        inferredType, "Failed to parse type [{}]. Type not registered.", type);
  }
  return inferredType;
}

std::pair<std::string, std::shared_ptr<const Type>> inferTypeWithSpaces(
    std::vector<std::string>& words,
    bool cannotHaveFieldName = false) {
  VELOX_CHECK_GE(words.size(), 2);
  const auto& fieldName = words[0];
  const auto allWords = folly::join(" ", words);
  // Fail if cannotHaveFieldName = true.
  auto type = typeFromString(allWords, cannotHaveFieldName);
  if (type) {
    return std::make_pair("", type);
  }
  return std::make_pair(
      fieldName, typeFromString(allWords.substr(fieldName.size() + 1)));
}

void toLowerUTF8(std::string& str) {
  std::locale const utf8("en_US.UTF-8");
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  std::wstring wideStr = converter.from_bytes(str);
  std::transform(wideStr.begin(), wideStr.end(), wideStr.begin(),
                  [&utf8](wchar_t c){ return std::towlower(c); });
  str = converter.to_bytes(wideStr);
}

} // namespace facebook::velox
