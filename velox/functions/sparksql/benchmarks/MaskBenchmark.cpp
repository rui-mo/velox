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

#include <folly/Benchmark.h>
#include <folly/init/Init.h>

#include "velox/benchmarks/ExpressionBenchmarkBuilder.h"
#include "velox/functions/sparksql/Register.h"

using namespace facebook;

using namespace facebook::velox;

int main(int argc, char** argv) {
  folly::Init init(&argc, &argv);
  memory::MemoryManager::initialize({});
  functions::sparksql::registerFunctions("");

  ExpressionBenchmarkBuilder benchmarkBuilder;
  const vector_size_t vectorSize = 1000;
  auto vectorMaker = benchmarkBuilder.vectorMaker();
  auto input = vectorMaker.flatVector<std::string>(vectorSize, [](auto row) {
    return std::to_string(row) + "abcdefg" + "ABCDEFGHIJKL" +
        "@@@****&&&&^^^^^^" + std::to_string(row * 1000) + "MNOPQRSTUVWXYZ";
  });

  // Constant vectors for replacing chars.
  auto upperCharsConstant = vectorMaker.constantVector(
      std::vector<std::optional<StringView>>(vectorSize, StringView("Q")));
  auto lowerCharsConstant = vectorMaker.constantVector(
      std::vector<std::optional<StringView>>(vectorSize, StringView("q")));
  auto digitCharsConstant = vectorMaker.constantVector(
      std::vector<std::optional<StringView>>(vectorSize, StringView("d")));
  auto otherCharsConstant = vectorMaker.constantVector(
      std::vector<std::optional<StringView>>(vectorSize, StringView("o")));

  // Flat vectors for replacing chars.
  auto upperCharsFlat = vectorMaker.flatVector<std::string>(
      vectorSize, [&](auto row) { return std::string(1, 'Q' + row % 26); });
  auto lowerCharsFlat = vectorMaker.flatVector<std::string>(
      vectorSize, [&](auto row) { return std::string(1, 'q' + row % 26); });
  auto digitCharsFlat = vectorMaker.flatVector<std::string>(
      vectorSize, [&](auto row) { return std::string(1, 'd' + row % 26); });
  auto otherCharsFlat = vectorMaker.flatVector<std::string>(
      vectorSize, [&](auto row) { return std::string(1, 'o' + row % 26); });

  benchmarkBuilder
      .addBenchmarkSet(
          "mask_function",
          vectorMaker.rowVector(
              {"input",
               "upper_chars_constant",
               "lower_chars_constant",
               "digit_chars_constant",
               "other_chars_constant",
               "upper_chars_flat",
               "lower_chars_flat",
               "digit_chars_flat",
               "other_chars_flat"},
              {input,
               upperCharsConstant,
               lowerCharsConstant,
               digitCharsConstant,
               otherCharsConstant,
               upperCharsFlat,
               lowerCharsFlat,
               digitCharsFlat,
               otherCharsFlat}))
      .addExpression(
          "constant",
          "mask(input, upper_chars_constant, lower_chars_constant, digit_chars_constant, other_chars_constant)")
      .addExpression(
          "flat",
          "mask(input, upper_chars_flat, lower_chars_flat, digit_chars_flat, other_chars_flat)")
      .withIterations(100);

  benchmarkBuilder.registerBenchmarks();
  folly::runBenchmarks();
  return 0;
}
