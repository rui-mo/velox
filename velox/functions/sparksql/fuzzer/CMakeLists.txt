# Copyright (c) Facebook, Inc. and its affiliates.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Set up Spark connect
file(
  GLOB PROTO_FILES
  RELATIVE ${PROJECT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/proto/spark/connect/*.proto)
foreach(PROTO ${PROTO_FILES})
  get_filename_component(PROTO_DIR ${PROTO} DIRECTORY)
  get_filename_component(PROTO_NAME ${PROTO} NAME_WE)
  set(PROTO_OUTPUT_DIR ${PROJECT_BINARY_DIR}/${PROTO_DIR})
  list(APPEND PROTO_SRCS "${PROTO_OUTPUT_DIR}/${PROTO_NAME}.pb.cc")
  list(APPEND PROTO_HDRS "${PROTO_OUTPUT_DIR}/${PROTO_NAME}.pb.h")
  list(APPEND GRPC_SRCS "${PROTO_OUTPUT_DIR}/${PROTO_NAME}.grpc.pb.cc")
  list(APPEND GRPC_HDRS "${PROTO_OUTPUT_DIR}/${PROTO_NAME}.grpc.pb.h")
  list(APPEND PROTO_FILES_FULL
       "${PROJECT_SOURCE_DIR}/${PROTO_DIR}/${PROTO_NAME}.proto")
endforeach()

set(PROTO_OUTPUT_FILES ${PROTO_HDRS} ${PROTO_SRCS})
set_source_files_properties(${PROTO_OUTPUT_FILES} PROPERTIES GENERATED TRUE)

set(GRPC_OUTPUT_FILES ${GRPC_HDRS} ${GRPC_SRCS})
set_source_files_properties(${GRPC_OUTPUT_FILES} PROPERTIES GENERATED TRUE)

# Generate Spark connect hearders and sources
add_custom_command(
  OUTPUT ${PROTO_OUTPUT_FILES}
  COMMAND
    ${Protobuf_PROTOC_EXECUTABLE} --proto_path ${CMAKE_SOURCE_DIR}/ --proto_path
    ${Protobuf_INCLUDE_DIRS} --cpp_out ${CMAKE_BINARY_DIR} ${PROTO_FILES_FULL}
  DEPENDS ${Protobuf_PROTOC_EXECUTABLE}
  COMMENT "Running PROTO compiler"
  VERBATIM)
add_custom_target(spark_connect_proto ALL DEPENDS ${PROTO_OUTPUT_FILES})
# add_dependencies(spark_connect_proto protobuf::libprotobuf)

find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
# Generate grpc headers and sources
add_custom_command(
  OUTPUT ${GRPC_OUTPUT_FILES}
  COMMAND
    ${Protobuf_PROTOC_EXECUTABLE} --proto_path ${CMAKE_SOURCE_DIR}/ --proto_path
    ${Protobuf_INCLUDE_DIRS} --grpc_out=${CMAKE_BINARY_DIR}
    --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} ${PROTO_FILES_FULL}
  DEPENDS ${Protobuf_PROTOC_EXECUTABLE}
  COMMENT "Running gRPC C++ protocol buffer compiler"
  VERBATIM)
add_custom_target(spark_connect_grpc_proto ALL DEPENDS ${GRPC_OUTPUT_FILES})

set(SRCS ${PROTO_SRCS} ${GRPC_SRCS} SparkQueryRunner.cpp)

add_library(velox_spark_query_runner ${SRCS})
target_include_directories(velox_spark_query_runner
                           PUBLIC ${PROJECT_BINARY_DIR}/)

include(${PROJECT_SOURCE_DIR}/CMake/Findgrpc.cmake)
target_link_libraries(
  velox_spark_query_runner
  velox_fuzzer_util
  velox_exec_test_lib
  velox_arrow_bridge
  grpc::grpc++
  grpc::grpc
  gpr::gpr
  protobuf::libprotobuf
  z::z
  re2::re2
  address_sorting::address_sorting
  cares::cares
  boringssl::ssl
  boringssl::crypto
  pthread
  arrow
  lz4::lz4
  zstd::zstd
  Snappy::snappy)

add_executable(spark_aggregation_fuzzer_test SparkAggregationFuzzerTest.cpp)

target_link_libraries(
  spark_aggregation_fuzzer_test
  velox_aggregation_fuzzer
  velox_aggregation_fuzzer_base
  velox_functions_spark_aggregates
  velox_spark_query_runner
  velox_fuzzer_util
  velox_window
  velox_vector_test_lib
  gtest
  gtest_main
  grpc::grpc++
  grpc::grpc
  gpr::gpr
  protobuf::libprotobuf
  z::z
  re2::re2
  address_sorting::address_sorting
  cares::cares
  upb::upb
  boringssl::ssl
  boringssl::crypto
  absl::absl_statusor
  absl::absl_status
  absl::absl_cordz_info
  absl::absl_cord
  absl::absl_symbolize
  absl::absl_demangle_internal
  absl::absl_flags_parse
  absl::absl_flags
  absl::absl_cord_internal
  absl::absl_bad_optional_access
  absl::absl_throw_delegate
  absl::absl_bad_variant_access
  absl::absl_random_internal_pool_urbg
  absl::absl_random_internal_randen
  absl::absl_random_internal_seed_material
  absl::absl_random_seed_gen_exception
  absl::absl_random_internal_randen_hwaes
  absl::absl_random_internal_randen_slow
  absl::absl_graphcycles_internal
  absl::absl_base
  absl::absl_malloc_internal
  absl::absl_examine_stack
  absl::absl_debugging_internal
  absl::absl_exponential_biased
  absl::absl_cordz_handle
  absl::absl_log_severity
  absl::absl_stacktrace
  absl::absl_time
  absl::absl_cordz_functions
  absl::absl_raw_logging_internal
  absl::absl_str_format_internal
  absl::absl_strerror
  absl::absl_strings
  absl::absl_strings_internal
  absl::absl_raw_hash_set
  absl::absl_hashtablez_sampler
  absl::absl_hash
  absl::absl_city
  absl::absl_int128
  absl::absl_low_level_hash
  absl::absl_random_internal_randen_hwaes_impl
  absl::absl_random_internal_platform
  absl::absl_flags_internal
  absl::absl_flags_usage_internal
  absl::absl_flags_reflection
  absl::absl_flags_commandlineflag
  absl::absl_flags_private_handle_accessor
  absl::absl_flags_commandlineflag_internal
  absl::absl_flags_config
  absl::absl_flags_program_name
  absl::absl_flags_marshalling
  absl::absl_synchronization
  absl::absl_time_zone
  absl::absl_spinlock_wait
  pthread
  arrow
  lz4::lz4
  zstd::zstd
  Snappy::snappy)

if(${VELOX_BUILD_TESTING})
  add_subdirectory(tests)
endif()
