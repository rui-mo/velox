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

target_link_libraries(
  velox_spark_query_runner
  velox_fuzzer_util
  velox_exec_test_lib
  velox_arrow_bridge
  gRPC::grpc++
  gRPC::grpc
  gRPC::gpr
  protobuf::libprotobuf
  ZLIB::ZLIB
  re2::re2
  gRPC::address_sorting
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
  gRPC::grpc++
  gRPC::grpc
  gRPC::gpr
  protobuf::libprotobuf
  ZLIB::ZLIB
  re2::re2
  gRPC::address_sorting
  cares::cares
  gRPC::upb
  boringssl::ssl
  boringssl::crypto
  absl::statusor
  absl::status
  absl::cordz_info
  absl::cord
  absl::symbolize
  absl::demangle_internal
  absl::flags_parse
  absl::cord_internal
  absl::bad_optional_access
  absl::throw_delegate
  absl::bad_variant_access
  absl::random_internal_pool_urbg
  absl::random_internal_randen
  absl::random_internal_seed_material
  absl::random_seed_gen_exception
  absl::random_internal_randen_hwaes
  absl::random_internal_randen_slow
  absl::graphcycles_internal
  absl::kernel_timeout_internal
  absl::base
  absl::malloc_internal
  absl::examine_stack
  absl::debugging_internal
  absl::exponential_biased
  absl::cordz_handle
  absl::log_severity
  absl::stacktrace
  absl::time
  absl::cordz_functions
  absl::raw_logging_internal
  absl::str_format_internal
  absl::strerror
  absl::crc_cord_state
  absl::crc32c
  absl::crc_internal
  absl::strings
  absl::strings_internal
  absl::raw_hash_set
  absl::hashtablez_sampler
  absl::hash
  absl::city
  absl::int128
  absl::low_level_hash
  absl::random_internal_randen_hwaes_impl
  absl::random_internal_platform
  absl::flags_internal
  absl::flags_usage_internal
  absl::flags_reflection
  absl::flags_commandlineflag
  absl::flags_private_handle_accessor
  absl::flags_commandlineflag_internal
  absl::flags_config
  absl::flags_program_name
  absl::flags_marshalling
  absl::synchronization
  absl::time_zone
  absl::spinlock_wait
  pthread
  arrow
  lz4::lz4
  zstd::zstd
  Snappy::snappy)

if(${VELOX_BUILD_TESTING})
  add_subdirectory(tests)
endif()
