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

add_executable(velox_spark_query_runner_test SparkQueryRunnerTest.cpp)

add_test(
  NAME velox_spark_query_runner_test
  COMMAND velox_spark_query_runner_test
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
  velox_spark_query_runner_test
  velox_dwio_common
  velox_dwio_common_exception
  velox_dwio_common_test_utils
  velox_aggregates
  velox_functions_spark_window
  velox_functions_spark_aggregates
  velox_exec
  velox_exec_test_lib
  velox_spark_query_runner
  velox_fuzzer_util
  velox_functions_spark
  velox_functions_lib
  velox_functions_test_lib
  velox_window
  velox_hive_connector
  velox_memory
  velox_serialization
  velox_test_util
  velox_parse_expression
  velox_type
  velox_parse_utils
  velox_vector
  velox_vector_fuzzer
  velox_presto_serializer
  velox_dwio_dwrf_writer
  Boost::atomic
  Boost::context
  Boost::date_time
  Boost::filesystem
  Boost::program_options
  Boost::regex
  Boost::thread
  Boost::system
  gtest
  gtest_main
  gmock
  Folly::folly
  gflags::gflags
  glog::glog
  fmt::fmt
  ${FILESYSTEM}
  gRPC::grpc++
  gRPC::grpc
  gRPC::gpr
  protobuf::libprotobuf
  ZLIB::ZLIB
  re2::re2
  gRPC::address_sorting
  c-ares::cares
  gRPC::upb
  OpenSSL::SSL
  OpenSSL::Crypto
  pthread
  arrow
  lz4::lz4
  zstd::zstd
  Snappy::snappy)
