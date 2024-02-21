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

#include <re2/re2.h>
#include <utility>

#include "velox/expression/VectorFunction.h"
#include "velox/expression/VectorWriters.h"

namespace facebook::velox::functions::sparksql {
namespace {

class Split final : public exec::VectorFunction {
 public:
  void apply(
      const SelectivityVector& rows,
      std::vector<VectorPtr>& args,
      const TypePtr& /* outputType */,
      exec::EvalCtx& context,
      VectorPtr& result) const override {
    exec::LocalDecodedVector input(context, *args[0], rows);

    BaseVector::ensureWritable(rows, ARRAY(VARCHAR()), context.pool(), result);
    exec::VectorWriter<Array<Varchar>> resultWriter;
    resultWriter.init(*result->as<ArrayVector>());

    // Fast path for pattern and limit being constant.
    if (args[1]->isConstantEncoding() && args[2]->isConstantEncoding()) {
      // Adds brackets to the input pattern for sub-pattern extraction.
      const auto pattern =
          args[1]->asUnchecked<ConstantVector<StringView>>()->valueAt(0);
      const auto limit =
          args[2]->asUnchecked<ConstantVector<int32_t>>()->valueAt(0);
      if (pattern.size() == 0) {
        if (limit > 0) {
          rows.applyToSelected([&](vector_size_t row) {
            splitEmptyPattern<true>(
                input->valueAt<StringView>(row), row, resultWriter, limit);
          });
        } else {
          rows.applyToSelected([&](vector_size_t row) {
            splitEmptyPattern<false>(
                input->valueAt<StringView>(row), row, resultWriter);
          });
        }
      } else {
        const auto re = re2::RE2("(" + pattern.str() + ")");
        if (limit > 0) {
          rows.applyToSelected([&](vector_size_t row) {
            splitAndWrite<true>(
                input->valueAt<StringView>(row), re, row, resultWriter, limit);
          });
        } else {
          rows.applyToSelected([&](vector_size_t row) {
            splitAndWrite<false>(
                input->valueAt<StringView>(row), re, row, resultWriter);
          });
        }
      }
    } else {
      exec::LocalDecodedVector patterns(context, *args[1], rows);
      exec::LocalDecodedVector limits(context, *args[2], rows);

      rows.applyToSelected([&](vector_size_t row) {
        const auto pattern = patterns->valueAt<StringView>(row);
        const auto limit = limits->valueAt<int32_t>(row);
        if (pattern.size() == 0) {
          if (limit > 0) {
            splitEmptyPattern<true>(
                input->valueAt<StringView>(row), row, resultWriter, limit);
          } else {
            splitEmptyPattern<false>(
                input->valueAt<StringView>(row), row, resultWriter);
          }
        } else {
          const auto re = re2::RE2("(" + pattern.str() + ")");
          if (limit > 0) {
            splitAndWrite<true>(
                input->valueAt<StringView>(row), re, row, resultWriter, limit);
          } else {
            splitAndWrite<false>(
                input->valueAt<StringView>(row), re, row, resultWriter);
          }
        }
      });
    }
    resultWriter.finish();

    // Reference the input StringBuffers since we did not deep copy above.
    result->as<ArrayVector>()
        ->elements()
        ->as<FlatVector<StringView>>()
        ->acquireSharedStringBuffers(args[0].get());
  }

 private:
  // When pattern is empty, split each character.
  template <bool limited>
  void splitEmptyPattern(
      const StringView current,
      vector_size_t row,
      exec::VectorWriter<Array<Varchar>>& resultWriter,
      uint32_t limit = 0) const {
    resultWriter.setOffset(row);
    auto& arrayWriter = resultWriter.current();
    if (current.size() == 0) {
      arrayWriter.add_item().setNoCopy(StringView());
      resultWriter.commit();
      return;
    }

    const char* const begin = current.begin();
    const char* const end = current.end();
    const char* pos = begin;
    if constexpr (limited) {
      VELOX_DCHECK_GT(limit, 0);
      while (pos != end && pos - begin < limit - 1) {
        arrayWriter.add_item().setNoCopy(StringView(pos, 1));
        pos += 1;
      }
      if (pos < end) {
        arrayWriter.add_item().setNoCopy(StringView(pos, end - pos));
      }
    } else {
      while (pos != end) {
        arrayWriter.add_item().setNoCopy(StringView(pos, 1));
        pos += 1;
      }
    }
    resultWriter.commit();
  }

  // Split with a non-empty pattern.
  template <bool limited>
  void splitAndWrite(
      const StringView current,
      const re2::RE2& re,
      vector_size_t row,
      exec::VectorWriter<Array<Varchar>>& resultWriter,
      uint32_t limit = 0) const {
    resultWriter.setOffset(row);
    auto& arrayWriter = resultWriter.current();
    if (current.size() == 0) {
      arrayWriter.add_item().setNoCopy(StringView());
      resultWriter.commit();
      return;
    }

    const char* pos = current.begin();
    const char* const end = current.end();
    if constexpr (limited) {
      VELOX_DCHECK_GT(limit, 0);
      uint32_t numPieces = 0;
      while (pos != end && numPieces < limit - 1) {
        if (re2::StringPiece piece; re2::RE2::PartialMatch(
                re2::StringPiece(pos, end - pos), re, &piece)) {
          arrayWriter.add_item().setNoCopy(StringView(pos, piece.data() - pos));
          numPieces += 1;
          if (piece.end() == end) {
            // When the found delimiter is at the end of input string, keeps
            // one empty piece of string.
            arrayWriter.add_item().setNoCopy(StringView());
          }
          pos = piece.end();
        } else {
          arrayWriter.add_item().setNoCopy(StringView(pos, end - pos));
          pos = end;
        }
      }
      if (pos < end) {
        arrayWriter.add_item().setNoCopy(StringView(pos, end - pos));
      }
    } else {
      while (pos != end) {
        if (re2::StringPiece piece; re2::RE2::PartialMatch(
                re2::StringPiece(pos, end - pos), re, &piece)) {
          arrayWriter.add_item().setNoCopy(StringView(pos, piece.data() - pos));
          if (piece.end() == end) {
            arrayWriter.add_item().setNoCopy(StringView());
          }
          pos = piece.end();
        } else {
          arrayWriter.add_item().setNoCopy(StringView(pos, end - pos));
          pos = end;
        }
      }
    }
    resultWriter.commit();
  }
};

/// Returns split function.
/// @param inputArgs the inputs types (VARCHAR, VARCHAR, int32).
std::shared_ptr<exec::VectorFunction> createSplit(
    const std::string& /*name*/,
    const std::vector<exec::VectorFunctionArg>& inputArgs,
    const core::QueryConfig& /*config*/) {
  VELOX_USER_CHECK_EQ(
      inputArgs.size(), 3, "Three arguments are required for split function.");
  VELOX_USER_CHECK(
      inputArgs[0].type->isVarchar(),
      "The first argument should be of varchar type.");
  VELOX_USER_CHECK(
      inputArgs[1].type->isVarchar(),
      "The second argument should be of varchar type.");
  VELOX_USER_CHECK(
      inputArgs[2].type->kind() == TypeKind::INTEGER,
      "The third argument should be of integer type.");
  return std::make_shared<Split>();
}

std::vector<std::shared_ptr<exec::FunctionSignature>> signatures() {
  // varchar, varchar -> array(varchar)
  return {exec::FunctionSignatureBuilder()
              .returnType("array(varchar)")
              .argumentType("varchar")
              .argumentType("varchar")
              .argumentType("integer")
              .build()};
}

} // namespace

VELOX_DECLARE_STATEFUL_VECTOR_FUNCTION(
    udf_regexp_split,
    signatures(),
    createSplit);
} // namespace facebook::velox::functions::sparksql
