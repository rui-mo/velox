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
  absl::absl_kernel_timeout_internal
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
  absl::absl_crc_cord_state
  absl::absl_crc32c
  absl::absl_crc_internal
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
