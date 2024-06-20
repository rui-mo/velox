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

# Dependencies
include(FindPackageHandleStandardArgs)

find_library(grpc_lib grpc)
find_package_handle_standard_args(grpc REQUIRED_VARS grpc_lib)
if(grpc_FOUND)
  add_library(grpc::grpc STATIC IMPORTED)
  target_link_libraries(grpc::grpc INTERFACE ${grpc_lib})
  set_target_properties(grpc::grpc PROPERTIES IMPORTED_LOCATION "${grpc_lib}")
endif()

find_library(grpc++_lib grpc++)
find_package_handle_standard_args(grpc++ REQUIRED_VARS grpc++_lib)
if(grpc++_FOUND)
  add_library(grpc::grpc++ STATIC IMPORTED)
  target_link_libraries(grpc::grpc++ INTERFACE ${grpc++_lib})
  set_target_properties(grpc::grpc++ PROPERTIES IMPORTED_LOCATION
                                                "${grpc++_lib}")
endif()

find_library(gpr_lib gpr)
find_package_handle_standard_args(gpr REQUIRED_VARS gpr_lib)
if(gpr_FOUND)
  add_library(gpr::gpr STATIC IMPORTED)
  target_link_libraries(gpr::gpr INTERFACE ${gpr_lib})
  set_target_properties(gpr::gpr PROPERTIES IMPORTED_LOCATION "${gpr_lib}")
endif()

find_library(z_lib z)
find_package_handle_standard_args(z REQUIRED_VARS z_lib)
if(z_FOUND)
  add_library(z::z STATIC IMPORTED)
  target_link_libraries(z::z INTERFACE ${z_lib})
  set_target_properties(z::z PROPERTIES IMPORTED_LOCATION "${z_lib}")
endif()

find_library(address_sorting_lib address_sorting)
find_package_handle_standard_args(address_sorting
                                  REQUIRED_VARS address_sorting_lib)
if(address_sorting_FOUND)
  add_library(address_sorting::address_sorting STATIC IMPORTED)
  target_link_libraries(address_sorting::address_sorting
                        INTERFACE ${address_sorting_lib})
  set_target_properties(address_sorting::address_sorting
                        PROPERTIES IMPORTED_LOCATION "${address_sorting_lib}")
endif()

find_library(cares_lib cares)
find_package_handle_standard_args(cares REQUIRED_VARS cares_lib)
if(cares_FOUND)
  add_library(cares::cares STATIC IMPORTED)
  target_link_libraries(cares::cares INTERFACE ${cares_lib})
  set_target_properties(cares::cares PROPERTIES IMPORTED_LOCATION
                                                "${cares_lib}")
endif()

find_library(upb_lib upb)
find_package_handle_standard_args(upb REQUIRED_VARS upb_lib)
if(upb_FOUND)
  add_library(upb::upb STATIC IMPORTED)
  target_link_libraries(upb::upb INTERFACE ${upb_lib})
  set_target_properties(upb::upb PROPERTIES IMPORTED_LOCATION "${upb_lib}")
endif()

find_library(ssl_lib ssl)
find_package_handle_standard_args(ssl REQUIRED_VARS ssl_lib)
if(ssl_FOUND)
  add_library(boringssl::ssl STATIC IMPORTED)
  target_link_libraries(boringssl::ssl INTERFACE ${ssl_lib})
  set_target_properties(boringssl::ssl PROPERTIES IMPORTED_LOCATION
                                                  "${ssl_lib}")
endif()

find_library(crypto_lib crypto)
find_package_handle_standard_args(crypto REQUIRED_VARS crypto_lib)
if(crypto_FOUND)
  add_library(boringssl::crypto STATIC IMPORTED)
  target_link_libraries(boringssl::crypto INTERFACE ${crypto_lib})
  set_target_properties(boringssl::crypto PROPERTIES IMPORTED_LOCATION
                                                     "${crypto_lib}")
endif()

find_library(absl_synchronization_lib absl_synchronization)
find_package_handle_standard_args(absl_synchronization
                                  REQUIRED_VARS absl_synchronization_lib)
if(absl_synchronization_FOUND)
  add_library(absl::absl_synchronization STATIC IMPORTED)
  target_link_libraries(absl::absl_synchronization
                        INTERFACE ${absl_synchronization_lib})
  set_target_properties(
    absl::absl_synchronization PROPERTIES IMPORTED_LOCATION
                                          "${absl_synchronization_lib}")
endif()

find_library(absl_time_zone_lib absl_time_zone)
find_package_handle_standard_args(absl_time_zone
                                  REQUIRED_VARS absl_time_zone_lib)
if(absl_time_zone_FOUND)
  add_library(absl::absl_time_zone STATIC IMPORTED)
  target_link_libraries(absl::absl_time_zone INTERFACE ${absl_time_zone_lib})
  set_target_properties(absl::absl_time_zone PROPERTIES IMPORTED_LOCATION
                                                        "${absl_time_zone_lib}")
endif()

find_library(absl_statusor_lib absl_statusor)
find_package_handle_standard_args(absl_statusor REQUIRED_VARS absl_statusor_lib)
if(absl_statusor_FOUND)
  add_library(absl::absl_statusor STATIC IMPORTED)
  target_link_libraries(absl::absl_statusor INTERFACE ${absl_statusor_lib})
  set_target_properties(absl::absl_statusor PROPERTIES IMPORTED_LOCATION
                                                       "${absl_statusor_lib}")
endif()

find_library(absl_status_lib absl_status)
find_package_handle_standard_args(absl_status REQUIRED_VARS absl_status_lib)
if(absl_status_FOUND)
  add_library(absl::absl_status STATIC IMPORTED)
  target_link_libraries(absl::absl_status INTERFACE ${absl_status_lib})
  set_target_properties(absl::absl_status PROPERTIES IMPORTED_LOCATION
                                                     "${absl_status_lib}")
endif()

find_library(absl_cordz_info_lib absl_cordz_info)
find_package_handle_standard_args(absl_cordz_info
                                  REQUIRED_VARS absl_cordz_info_lib)
if(absl_cordz_info_FOUND)
  add_library(absl::absl_cordz_info STATIC IMPORTED)
  target_link_libraries(absl::absl_cordz_info INTERFACE ${absl_cordz_info_lib})
  set_target_properties(absl::absl_cordz_info
                        PROPERTIES IMPORTED_LOCATION "${absl_cordz_info_lib}")
endif()

find_library(absl_cord_lib absl_cord)
find_package_handle_standard_args(absl_cord REQUIRED_VARS absl_cord_lib)
if(absl_cord_FOUND)
  add_library(absl::absl_cord STATIC IMPORTED)
  target_link_libraries(absl::absl_cord INTERFACE ${absl_cord_lib})
  set_target_properties(absl::absl_cord PROPERTIES IMPORTED_LOCATION
                                                   "${absl_cord_lib}")
endif()

find_library(absl_symbolize_lib absl_symbolize)
find_package_handle_standard_args(absl_symbolize
                                  REQUIRED_VARS absl_symbolize_lib)
if(absl_symbolize_FOUND)
  add_library(absl::absl_symbolize STATIC IMPORTED)
  target_link_libraries(absl::absl_symbolize INTERFACE ${absl_symbolize_lib})
  set_target_properties(absl::absl_symbolize PROPERTIES IMPORTED_LOCATION
                                                        "${absl_symbolize_lib}")
endif()

find_library(absl_demangle_internal_lib absl_demangle_internal)
find_package_handle_standard_args(absl_demangle_internal
                                  REQUIRED_VARS absl_demangle_internal_lib)
if(absl_demangle_internal_FOUND)
  add_library(absl::absl_demangle_internal STATIC IMPORTED)
  target_link_libraries(absl::absl_demangle_internal
                        INTERFACE ${absl_demangle_internal_lib})
  set_target_properties(
    absl::absl_demangle_internal PROPERTIES IMPORTED_LOCATION
                                            "${absl_demangle_internal_lib}")
endif()

find_library(absl_flags_parse_lib absl_flags_parse)
find_package_handle_standard_args(absl_flags_parse
                                  REQUIRED_VARS absl_flags_parse_lib)
if(absl_flags_parse_FOUND)
  add_library(absl::absl_flags_parse STATIC IMPORTED)
  target_link_libraries(absl::absl_flags_parse
                        INTERFACE ${absl_flags_parse_lib})
  set_target_properties(absl::absl_flags_parse
                        PROPERTIES IMPORTED_LOCATION "${absl_flags_parse_lib}")
endif()

find_library(absl_flags_lib absl_flags)
find_package_handle_standard_args(absl_flags REQUIRED_VARS absl_flags_lib)
if(absl_flags_FOUND)
  add_library(absl::absl_flags STATIC IMPORTED)
  target_link_libraries(absl::absl_flags INTERFACE ${absl_flags_lib})
  set_target_properties(absl::absl_flags PROPERTIES IMPORTED_LOCATION
                                                    "${absl_flags_lib}")
endif()

find_library(absl_base_lib absl_base)
find_package_handle_standard_args(absl_base REQUIRED_VARS absl_base_lib)
if(absl_base_FOUND)
  add_library(absl::absl_base STATIC IMPORTED)
  target_link_libraries(absl::absl_base INTERFACE ${absl_base_lib})
  set_target_properties(absl::absl_base PROPERTIES IMPORTED_LOCATION
                                                   "${absl_base_lib}")
endif()

find_library(absl_cord_internal_lib absl_cord_internal)
find_package_handle_standard_args(absl_cord_internal
                                  REQUIRED_VARS absl_cord_internal_lib)
if(absl_cord_internal_FOUND)
  add_library(absl::absl_cord_internal STATIC IMPORTED)
  target_link_libraries(absl::absl_cord_internal
                        INTERFACE ${absl_cord_internal_lib})
  set_target_properties(
    absl::absl_cord_internal PROPERTIES IMPORTED_LOCATION
                                        "${absl_cord_internal_lib}")
endif()

find_library(absl_malloc_internal_lib absl_malloc_internal)
find_package_handle_standard_args(absl_malloc_internal
                                  REQUIRED_VARS absl_malloc_internal_lib)
if(absl_malloc_internal_FOUND)
  add_library(absl::absl_malloc_internal STATIC IMPORTED)
  target_link_libraries(absl::absl_malloc_internal
                        INTERFACE ${absl_malloc_internal_lib})
  set_target_properties(
    absl::absl_malloc_internal PROPERTIES IMPORTED_LOCATION
                                          "${absl_malloc_internal_lib}")
endif()

find_library(absl_bad_optional_access_lib absl_bad_optional_access)
find_package_handle_standard_args(absl_bad_optional_access
                                  REQUIRED_VARS absl_bad_optional_access_lib)
if(absl_bad_optional_access_FOUND)
  add_library(absl::absl_bad_optional_access STATIC IMPORTED)
  target_link_libraries(absl::absl_bad_optional_access
                        INTERFACE ${absl_bad_optional_access_lib})
  set_target_properties(
    absl::absl_bad_optional_access PROPERTIES IMPORTED_LOCATION
                                              "${absl_bad_optional_access_lib}")
endif()

find_library(absl_throw_delegate_lib absl_throw_delegate)
find_package_handle_standard_args(absl_throw_delegate
                                  REQUIRED_VARS absl_throw_delegate_lib)
if(absl_throw_delegate_FOUND)
  add_library(absl::absl_throw_delegate STATIC IMPORTED)
  target_link_libraries(absl::absl_throw_delegate
                        INTERFACE ${absl_throw_delegate_lib})
  set_target_properties(
    absl::absl_throw_delegate PROPERTIES IMPORTED_LOCATION
                                         "${absl_throw_delegate_lib}")
endif()

find_library(absl_bad_variant_access_lib absl_bad_variant_access)
find_package_handle_standard_args(absl_bad_variant_access
                                  REQUIRED_VARS absl_bad_variant_access_lib)
if(absl_bad_variant_access_FOUND)
  add_library(absl::absl_bad_variant_access STATIC IMPORTED)
  target_link_libraries(absl::absl_bad_variant_access
                        INTERFACE ${absl_bad_variant_access_lib})
  set_target_properties(
    absl::absl_bad_variant_access PROPERTIES IMPORTED_LOCATION
                                             "${absl_bad_variant_access_lib}")
endif()

find_library(absl_random_internal_pool_urbg_lib absl_random_internal_pool_urbg)
find_package_handle_standard_args(
  absl_random_internal_pool_urbg
  REQUIRED_VARS absl_random_internal_pool_urbg_lib)
if(absl_random_internal_pool_urbg_FOUND)
  add_library(absl::absl_random_internal_pool_urbg STATIC IMPORTED)
  target_link_libraries(absl::absl_random_internal_pool_urbg
                        INTERFACE ${absl_random_internal_pool_urbg_lib})
  set_target_properties(
    absl::absl_random_internal_pool_urbg
    PROPERTIES IMPORTED_LOCATION "${absl_random_internal_pool_urbg_lib}")
endif()

find_library(absl_random_internal_randen_lib absl_random_internal_randen)
find_package_handle_standard_args(absl_random_internal_randen
                                  REQUIRED_VARS absl_random_internal_randen_lib)
if(absl_random_internal_randen_FOUND)
  add_library(absl::absl_random_internal_randen STATIC IMPORTED)
  target_link_libraries(absl::absl_random_internal_randen
                        INTERFACE ${absl_random_internal_randen_lib})
  set_target_properties(
    absl::absl_random_internal_randen
    PROPERTIES IMPORTED_LOCATION "${absl_random_internal_randen_lib}")
endif()

find_library(absl_random_internal_seed_material_lib
             absl_random_internal_seed_material)
find_package_handle_standard_args(
  absl_random_internal_seed_material
  REQUIRED_VARS absl_random_internal_seed_material_lib)
if(absl_random_internal_seed_material_FOUND)
  add_library(absl::absl_random_internal_seed_material STATIC IMPORTED)
  target_link_libraries(absl::absl_random_internal_seed_material
                        INTERFACE ${absl_random_internal_seed_material_lib})
  set_target_properties(
    absl::absl_random_internal_seed_material
    PROPERTIES IMPORTED_LOCATION "${absl_random_internal_seed_material_lib}")
endif()

find_library(absl_random_seed_gen_exception_lib absl_random_seed_gen_exception)
find_package_handle_standard_args(
  absl_random_seed_gen_exception
  REQUIRED_VARS absl_random_seed_gen_exception_lib)
if(absl_random_seed_gen_exception_FOUND)
  add_library(absl::absl_random_seed_gen_exception STATIC IMPORTED)
  target_link_libraries(absl::absl_random_seed_gen_exception
                        INTERFACE ${absl_random_seed_gen_exception_lib})
  set_target_properties(
    absl::absl_random_seed_gen_exception
    PROPERTIES IMPORTED_LOCATION "${absl_random_seed_gen_exception_lib}")
endif()

find_library(absl_random_internal_randen_hwaes_lib
             absl_random_internal_randen_hwaes)
find_package_handle_standard_args(
  absl_random_internal_randen_hwaes
  REQUIRED_VARS absl_random_internal_randen_hwaes_lib)
if(absl_random_internal_randen_hwaes_FOUND)
  add_library(absl::absl_random_internal_randen_hwaes STATIC IMPORTED)
  target_link_libraries(absl::absl_random_internal_randen_hwaes
                        INTERFACE ${absl_random_internal_randen_hwaes_lib})
  set_target_properties(
    absl::absl_random_internal_randen_hwaes
    PROPERTIES IMPORTED_LOCATION "${absl_random_internal_randen_hwaes_lib}")
endif()

find_library(absl_random_internal_randen_slow_lib
             absl_random_internal_randen_slow)
find_package_handle_standard_args(
  absl_random_internal_randen_slow
  REQUIRED_VARS absl_random_internal_randen_slow_lib)
if(absl_random_internal_randen_slow_FOUND)
  add_library(absl::absl_random_internal_randen_slow STATIC IMPORTED)
  target_link_libraries(absl::absl_random_internal_randen_slow
                        INTERFACE ${absl_random_internal_randen_slow_lib})
  set_target_properties(
    absl::absl_random_internal_randen_slow
    PROPERTIES IMPORTED_LOCATION "${absl_random_internal_randen_slow_lib}")
endif()

find_library(absl_graphcycles_internal_lib absl_graphcycles_internal)
find_package_handle_standard_args(absl_graphcycles_internal
                                  REQUIRED_VARS absl_graphcycles_internal_lib)
if(absl_graphcycles_internal_FOUND)
  add_library(absl::absl_graphcycles_internal STATIC IMPORTED)
  target_link_libraries(absl::absl_graphcycles_internal
                        INTERFACE ${absl_graphcycles_internal_lib})
  set_target_properties(
    absl::absl_graphcycles_internal
    PROPERTIES IMPORTED_LOCATION "${absl_graphcycles_internal_lib}")
endif()

find_library(absl_examine_stack_lib absl_examine_stack)
find_package_handle_standard_args(absl_examine_stack
                                  REQUIRED_VARS absl_examine_stack_lib)
if(absl_examine_stack_FOUND)
  add_library(absl::absl_examine_stack STATIC IMPORTED)
  target_link_libraries(absl::absl_examine_stack
                        INTERFACE ${absl_examine_stack_lib})
  set_target_properties(
    absl::absl_examine_stack PROPERTIES IMPORTED_LOCATION
                                        "${absl_examine_stack_lib}")
endif()

find_library(absl_stacktrace_lib absl_stacktrace)
find_package_handle_standard_args(absl_stacktrace
                                  REQUIRED_VARS absl_stacktrace_lib)
if(absl_stacktrace_FOUND)
  add_library(absl::absl_stacktrace STATIC IMPORTED)
  target_link_libraries(absl::absl_stacktrace INTERFACE ${absl_stacktrace_lib})
  set_target_properties(absl::absl_stacktrace
                        PROPERTIES IMPORTED_LOCATION "${absl_stacktrace_lib}")
endif()

find_library(absl_debugging_internal_lib absl_debugging_internal)
find_package_handle_standard_args(absl_debugging_internal
                                  REQUIRED_VARS absl_debugging_internal_lib)
if(absl_debugging_internal_FOUND)
  add_library(absl::absl_debugging_internal STATIC IMPORTED)
  target_link_libraries(absl::absl_debugging_internal
                        INTERFACE ${absl_debugging_internal_lib})
  set_target_properties(
    absl::absl_debugging_internal PROPERTIES IMPORTED_LOCATION
                                             "${absl_debugging_internal_lib}")
endif()

find_library(absl_cordz_functions_lib absl_cordz_functions)
find_package_handle_standard_args(absl_cordz_functions
                                  REQUIRED_VARS absl_cordz_functions_lib)
if(absl_cordz_functions_FOUND)
  add_library(absl::absl_cordz_functions STATIC IMPORTED)
  target_link_libraries(absl::absl_cordz_functions
                        INTERFACE ${absl_cordz_functions_lib})
  set_target_properties(
    absl::absl_cordz_functions PROPERTIES IMPORTED_LOCATION
                                          "${absl_cordz_functions_lib}")
endif()

find_library(absl_exponential_biased_lib absl_exponential_biased)
find_package_handle_standard_args(absl_exponential_biased
                                  REQUIRED_VARS absl_exponential_biased_lib)
if(absl_exponential_biased_FOUND)
  add_library(absl::absl_exponential_biased STATIC IMPORTED)
  target_link_libraries(absl::absl_exponential_biased
                        INTERFACE ${absl_exponential_biased_lib})
  set_target_properties(
    absl::absl_exponential_biased PROPERTIES IMPORTED_LOCATION
                                             "${absl_exponential_biased_lib}")
endif()

find_library(absl_cordz_handle_lib absl_cordz_handle)
find_package_handle_standard_args(absl_cordz_handle
                                  REQUIRED_VARS absl_cordz_handle_lib)
if(absl_cordz_handle_FOUND)
  add_library(absl::absl_cordz_handle STATIC IMPORTED)
  target_link_libraries(absl::absl_cordz_handle
                        INTERFACE ${absl_cordz_handle_lib})
  set_target_properties(absl::absl_cordz_handle
                        PROPERTIES IMPORTED_LOCATION "${absl_cordz_handle_lib}")
endif()

find_library(absl_raw_logging_internal_lib absl_raw_logging_internal)
find_package_handle_standard_args(absl_raw_logging_internal
                                  REQUIRED_VARS absl_raw_logging_internal_lib)
if(absl_raw_logging_internal_FOUND)
  add_library(absl::absl_raw_logging_internal STATIC IMPORTED)
  target_link_libraries(absl::absl_raw_logging_internal
                        INTERFACE ${absl_raw_logging_internal_lib})
  set_target_properties(
    absl::absl_raw_logging_internal
    PROPERTIES IMPORTED_LOCATION "${absl_raw_logging_internal_lib}")
endif()

find_library(absl_log_severity_lib absl_log_severity)
find_package_handle_standard_args(absl_log_severity
                                  REQUIRED_VARS absl_log_severity_lib)
if(absl_log_severity_FOUND)
  add_library(absl::absl_log_severity STATIC IMPORTED)
  target_link_libraries(absl::absl_log_severity
                        INTERFACE ${absl_log_severity_lib})
  set_target_properties(absl::absl_log_severity
                        PROPERTIES IMPORTED_LOCATION "${absl_log_severity_lib}")
endif()

find_library(absl_time_lib absl_time)
find_package_handle_standard_args(absl_time REQUIRED_VARS absl_time_lib)
if(absl_time_FOUND)
  add_library(absl::absl_time STATIC IMPORTED)
  target_link_libraries(absl::absl_time INTERFACE ${absl_time_lib})
  set_target_properties(absl::absl_time PROPERTIES IMPORTED_LOCATION
                                                   "${absl_time_lib}")
endif()

find_library(absl_str_format_internal_lib absl_str_format_internal)
find_package_handle_standard_args(absl_str_format_internal
                                  REQUIRED_VARS absl_str_format_internal_lib)
if(absl_str_format_internal_FOUND)
  add_library(absl::absl_str_format_internal STATIC IMPORTED)
  target_link_libraries(absl::absl_str_format_internal
                        INTERFACE ${absl_str_format_internal_lib})
  set_target_properties(
    absl::absl_str_format_internal PROPERTIES IMPORTED_LOCATION
                                              "${absl_str_format_internal_lib}")
endif()

find_library(absl_strerror_lib absl_strerror)
find_package_handle_standard_args(absl_strerror REQUIRED_VARS absl_strerror_lib)
if(absl_strerror_FOUND)
  add_library(absl::absl_strerror STATIC IMPORTED)
  target_link_libraries(absl::absl_strerror INTERFACE ${absl_strerror_lib})
  set_target_properties(absl::absl_strerror PROPERTIES IMPORTED_LOCATION
                                                       "${absl_strerror_lib}")
endif()

find_library(absl_strings_lib absl_strings)
find_package_handle_standard_args(absl_strings REQUIRED_VARS absl_strings_lib)
if(absl_strings_FOUND)
  add_library(absl::absl_strings STATIC IMPORTED)
  target_link_libraries(absl::absl_strings INTERFACE ${absl_strings_lib})
  set_target_properties(absl::absl_strings PROPERTIES IMPORTED_LOCATION
                                                      "${absl_strings_lib}")
endif()

find_library(absl_strings_internal_lib absl_strings_internal)
find_package_handle_standard_args(absl_strings_internal
                                  REQUIRED_VARS absl_strings_internal_lib)
if(absl_strings_internal_FOUND)
  add_library(absl::absl_strings_internal STATIC IMPORTED)
  target_link_libraries(absl::absl_strings_internal
                        INTERFACE ${absl_strings_internal_lib})
  set_target_properties(
    absl::absl_strings_internal PROPERTIES IMPORTED_LOCATION
                                           "${absl_strings_internal_lib}")
endif()

find_library(absl_raw_hash_set_lib absl_raw_hash_set)
find_package_handle_standard_args(absl_raw_hash_set
                                  REQUIRED_VARS absl_raw_hash_set_lib)
if(absl_raw_hash_set_FOUND)
  add_library(absl::absl_raw_hash_set STATIC IMPORTED)
  target_link_libraries(absl::absl_raw_hash_set
                        INTERFACE ${absl_raw_hash_set_lib})
  set_target_properties(absl::absl_raw_hash_set
                        PROPERTIES IMPORTED_LOCATION "${absl_raw_hash_set_lib}")
endif()

find_library(absl_hashtablez_sampler_lib absl_hashtablez_sampler)
find_package_handle_standard_args(absl_hashtablez_sampler
                                  REQUIRED_VARS absl_hashtablez_sampler_lib)
if(absl_hashtablez_sampler_FOUND)
  add_library(absl::absl_hashtablez_sampler STATIC IMPORTED)
  target_link_libraries(absl::absl_hashtablez_sampler
                        INTERFACE ${absl_hashtablez_sampler_lib})
  set_target_properties(
    absl::absl_hashtablez_sampler PROPERTIES IMPORTED_LOCATION
                                             "${absl_hashtablez_sampler_lib}")
endif()

find_library(absl_hash_lib absl_hash)
find_package_handle_standard_args(absl_hash REQUIRED_VARS absl_hash_lib)
if(absl_hash_FOUND)
  add_library(absl::absl_hash STATIC IMPORTED)
  target_link_libraries(absl::absl_hash INTERFACE ${absl_hash_lib})
  set_target_properties(absl::absl_hash PROPERTIES IMPORTED_LOCATION
                                                   "${absl_hash_lib}")
endif()

find_library(absl_city_lib absl_city)
find_package_handle_standard_args(absl_city REQUIRED_VARS absl_city_lib)
if(absl_city_FOUND)
  add_library(absl::absl_city STATIC IMPORTED)
  target_link_libraries(absl::absl_city INTERFACE ${absl_city_lib})
  set_target_properties(absl::absl_city PROPERTIES IMPORTED_LOCATION
                                                   "${absl_city_lib}")
endif()

find_library(absl_int128_lib absl_int128)
find_package_handle_standard_args(absl_int128 REQUIRED_VARS absl_int128_lib)
if(absl_int128_FOUND)
  add_library(absl::absl_int128 STATIC IMPORTED)
  target_link_libraries(absl::absl_int128 INTERFACE ${absl_int128_lib})
  set_target_properties(absl::absl_int128 PROPERTIES IMPORTED_LOCATION
                                                     "${absl_int128_lib}")
endif()

find_library(absl_low_level_hash_lib absl_low_level_hash)
find_package_handle_standard_args(absl_low_level_hash
                                  REQUIRED_VARS absl_low_level_hash_lib)
if(absl_low_level_hash_FOUND)
  add_library(absl::absl_low_level_hash STATIC IMPORTED)
  target_link_libraries(absl::absl_low_level_hash
                        INTERFACE ${absl_low_level_hash_lib})
  set_target_properties(
    absl::absl_low_level_hash PROPERTIES IMPORTED_LOCATION
                                         "${absl_low_level_hash_lib}")
endif()

find_library(absl_random_internal_randen_hwaes_impl_lib
             absl_random_internal_randen_hwaes_impl)
find_package_handle_standard_args(
  absl_random_internal_randen_hwaes_impl
  REQUIRED_VARS absl_random_internal_randen_hwaes_impl_lib)
if(absl_random_internal_randen_hwaes_impl_FOUND)
  add_library(absl::absl_random_internal_randen_hwaes_impl STATIC IMPORTED)
  target_link_libraries(absl::absl_random_internal_randen_hwaes_impl
                        INTERFACE ${absl_random_internal_randen_hwaes_impl_lib})
  set_target_properties(
    absl::absl_random_internal_randen_hwaes_impl
    PROPERTIES IMPORTED_LOCATION
               "${absl_random_internal_randen_hwaes_impl_lib}")
endif()

find_library(absl_random_internal_platform_lib absl_random_internal_platform)
find_package_handle_standard_args(
  absl_random_internal_platform REQUIRED_VARS absl_random_internal_platform_lib)
if(absl_random_internal_platform_FOUND)
  add_library(absl::absl_random_internal_platform STATIC IMPORTED)
  target_link_libraries(absl::absl_random_internal_platform
                        INTERFACE ${absl_random_internal_platform_lib})
  set_target_properties(
    absl::absl_random_internal_platform
    PROPERTIES IMPORTED_LOCATION "${absl_random_internal_platform_lib}")
endif()

find_library(absl_flags_internal_lib absl_flags_internal)
find_package_handle_standard_args(absl_flags_internal
                                  REQUIRED_VARS absl_flags_internal_lib)
if(absl_flags_internal_FOUND)
  add_library(absl::absl_flags_internal STATIC IMPORTED)
  target_link_libraries(absl::absl_flags_internal
                        INTERFACE ${absl_flags_internal_lib})
  set_target_properties(
    absl::absl_flags_internal PROPERTIES IMPORTED_LOCATION
                                         "${absl_flags_internal_lib}")
endif()

find_library(absl_flags_usage_internal_lib absl_flags_usage_internal)
find_package_handle_standard_args(absl_flags_usage_internal
                                  REQUIRED_VARS absl_flags_usage_internal_lib)
if(absl_flags_usage_internal_FOUND)
  add_library(absl::absl_flags_usage_internal STATIC IMPORTED)
  target_link_libraries(absl::absl_flags_usage_internal
                        INTERFACE ${absl_flags_usage_internal_lib})
  set_target_properties(
    absl::absl_flags_usage_internal
    PROPERTIES IMPORTED_LOCATION "${absl_flags_usage_internal_lib}")
endif()

find_library(absl_flags_reflection_lib absl_flags_reflection)
find_package_handle_standard_args(absl_flags_reflection
                                  REQUIRED_VARS absl_flags_reflection_lib)
if(absl_flags_usage_internal_FOUND)
  add_library(absl::absl_flags_reflection STATIC IMPORTED)
  target_link_libraries(absl::absl_flags_reflection
                        INTERFACE ${absl_flags_reflection_lib})
  set_target_properties(
    absl::absl_flags_reflection PROPERTIES IMPORTED_LOCATION
                                           "${absl_flags_reflection_lib}")
endif()

find_library(absl_flags_commandlineflag_lib absl_flags_commandlineflag)
find_package_handle_standard_args(absl_flags_commandlineflag
                                  REQUIRED_VARS absl_flags_commandlineflag_lib)
if(absl_flags_commandlineflag_FOUND)
  add_library(absl::absl_flags_commandlineflag STATIC IMPORTED)
  target_link_libraries(absl::absl_flags_commandlineflag
                        INTERFACE ${absl_flags_commandlineflag_lib})
  set_target_properties(
    absl::absl_flags_commandlineflag
    PROPERTIES IMPORTED_LOCATION "${absl_flags_commandlineflag_lib}")
endif()

find_library(absl_flags_private_handle_accessor_lib
             absl_flags_private_handle_accessor)
find_package_handle_standard_args(
  absl_flags_private_handle_accessor
  REQUIRED_VARS absl_flags_private_handle_accessor_lib)
if(absl_flags_private_handle_accessor_FOUND)
  add_library(absl::absl_flags_private_handle_accessor STATIC IMPORTED)
  target_link_libraries(absl::absl_flags_private_handle_accessor
                        INTERFACE ${absl_flags_private_handle_accessor_lib})
  set_target_properties(
    absl::absl_flags_private_handle_accessor
    PROPERTIES IMPORTED_LOCATION "${absl_flags_private_handle_accessor_lib}")
endif()

find_library(absl_flags_commandlineflag_internal_lib
             absl_flags_commandlineflag_internal)
find_package_handle_standard_args(
  absl_flags_commandlineflag_internal
  REQUIRED_VARS absl_flags_commandlineflag_internal_lib)
if(absl_flags_commandlineflag_internal_FOUND)
  add_library(absl::absl_flags_commandlineflag_internal STATIC IMPORTED)
  target_link_libraries(absl::absl_flags_commandlineflag_internal
                        INTERFACE ${absl_flags_commandlineflag_internal_lib})
  set_target_properties(
    absl::absl_flags_commandlineflag_internal
    PROPERTIES IMPORTED_LOCATION "${absl_flags_commandlineflag_internal_lib}")
endif()

find_library(absl_flags_config_lib absl_flags_config)
find_package_handle_standard_args(absl_flags_config
                                  REQUIRED_VARS absl_flags_config_lib)
if(absl_flags_config_FOUND)
  add_library(absl::absl_flags_config STATIC IMPORTED)
  target_link_libraries(absl::absl_flags_config
                        INTERFACE ${absl_flags_config_lib})
  set_target_properties(absl::absl_flags_config
                        PROPERTIES IMPORTED_LOCATION "${absl_flags_config_lib}")
endif()

find_library(absl_flags_program_name_lib absl_flags_program_name)
find_package_handle_standard_args(absl_flags_program_name
                                  REQUIRED_VARS absl_flags_program_name_lib)
if(absl_flags_program_name_FOUND)
  add_library(absl::absl_flags_program_name STATIC IMPORTED)
  target_link_libraries(absl::absl_flags_program_name
                        INTERFACE ${absl_flags_program_name_lib})
  set_target_properties(
    absl::absl_flags_program_name PROPERTIES IMPORTED_LOCATION
                                             "${absl_flags_program_name_lib}")
endif()

find_library(absl_flags_marshalling_lib absl_flags_marshalling)
find_package_handle_standard_args(absl_flags_marshalling
                                  REQUIRED_VARS absl_flags_marshalling_lib)
if(absl_flags_marshalling_FOUND)
  add_library(absl::absl_flags_marshalling STATIC IMPORTED)
  target_link_libraries(absl::absl_flags_marshalling
                        INTERFACE ${absl_flags_marshalling_lib})
  set_target_properties(
    absl::absl_flags_marshalling PROPERTIES IMPORTED_LOCATION
                                            "${absl_flags_marshalling_lib}")
endif()

find_library(absl_spinlock_wait_lib absl_spinlock_wait)
find_package_handle_standard_args(absl_spinlock_wait
                                  REQUIRED_VARS absl_spinlock_wait_lib)
if(absl_spinlock_wait_FOUND)
  add_library(absl::absl_spinlock_wait STATIC IMPORTED)
  target_link_libraries(absl::absl_spinlock_wait
                        INTERFACE ${absl_spinlock_wait_lib})
  set_target_properties(
    absl::absl_spinlock_wait PROPERTIES IMPORTED_LOCATION
                                        "${absl_spinlock_wait_lib}")
endif()
