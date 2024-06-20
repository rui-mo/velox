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

function(find_and_link_lib prefix lib_name)
  find_library(${lib_name}_lib ${lib_name})
  find_package_handle_standard_args(${lib_name} REQUIRED_VARS ${lib_name}_lib)
  if(${lib_name}_FOUND)
    add_library(${prefix}::${lib_name} STATIC IMPORTED)
    get_filename_component(lib_path ${${lib_name}_lib} ABSOLUTE)
    message(STATUS "Found lib: ${lib_value}")
    target_link_libraries(${prefix}::${lib_name} INTERFACE ${lib_path})
    set_target_properties(
      ${prefix}::${lib_name} PROPERTIES IMPORTED_LOCATION "${lib_path}")
  endif()
endfunction()

find_and_link_lib(grpc grpc)
find_and_link_lib(grpc grpc++)
find_and_link_lib(gpr gpr)
find_and_link_lib(z z)
find_and_link_lib(address_sorting address_sorting)
find_and_link_lib(cares cares)
find_and_link_lib(upb upb)
find_and_link_lib(boringssl ssl)
find_and_link_lib(boringssl crypto)

find_and_link_lib(absl absl_synchronization)
find_and_link_lib(absl absl_time_zone)
find_and_link_lib(absl absl_statusor)
find_and_link_lib(absl absl_status)
find_and_link_lib(absl absl_cordz_info)
find_and_link_lib(absl absl_cord)
find_and_link_lib(absl absl_symbolize)
find_and_link_lib(absl absl_demangle_internal)
find_and_link_lib(absl absl_flags_parse)
find_and_link_lib(absl absl_base)
find_and_link_lib(absl absl_cord_internal)
find_and_link_lib(absl absl_malloc_internal)
find_and_link_lib(absl absl_bad_optional_access)
find_and_link_lib(absl absl_throw_delegate)
find_and_link_lib(absl absl_bad_variant_access)
find_and_link_lib(absl absl_random_internal_pool_urbg)
find_and_link_lib(absl absl_random_internal_randen)
find_and_link_lib(absl absl_random_internal_seed_material)
find_and_link_lib(absl absl_random_seed_gen_exception)
find_and_link_lib(absl absl_random_internal_randen_hwaes)
find_and_link_lib(absl absl_random_internal_randen_slow)
find_and_link_lib(absl absl_graphcycles_internal)
find_and_link_lib(absl absl_examine_stack)
find_and_link_lib(absl absl_stacktrace)
find_and_link_lib(absl absl_debugging_internal)
find_and_link_lib(absl absl_cordz_functions)
find_and_link_lib(absl absl_exponential_biased)
find_and_link_lib(absl absl_cordz_handle)
find_and_link_lib(absl absl_raw_logging_internal)
find_and_link_lib(absl absl_log_severity)
find_and_link_lib(absl absl_time)
find_and_link_lib(absl absl_str_format_internal)
find_and_link_lib(absl absl_strerror)
find_and_link_lib(absl absl_strings)
find_and_link_lib(absl absl_strings_internal)
find_and_link_lib(absl absl_raw_hash_set)
find_and_link_lib(absl absl_hashtablez_sampler)
find_and_link_lib(absl absl_hash)
find_and_link_lib(absl absl_city)
find_and_link_lib(absl absl_int128)
find_and_link_lib(absl absl_low_level_hash)
find_and_link_lib(absl absl_random_internal_randen_hwaes_impl)
find_and_link_lib(absl absl_random_internal_platform)
find_and_link_lib(absl absl_flags_internal)
find_and_link_lib(absl absl_flags_usage_internal)
find_and_link_lib(absl absl_flags_reflection)
find_and_link_lib(absl absl_flags_commandlineflag)
find_and_link_lib(absl absl_flags_private_handle_accessor)
find_and_link_lib(absl absl_flags_commandlineflag_internal)
find_and_link_lib(absl absl_flags_config)
find_and_link_lib(absl absl_flags_program_name)
find_and_link_lib(absl absl_flags_marshalling)
find_and_link_lib(absl absl_spinlock_wait)
find_and_link_lib(absl absl_kernel_timeout_internal)
find_and_link_lib(absl absl_crc_cord_state)
find_and_link_lib(absl absl_crc32c)
find_and_link_lib(absl absl_crc_internal)