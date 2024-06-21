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
include_guard(GLOBAL)

set(VELOX_BORINGSSL_BUILD_VERSION fips-20220613)
set(VELOX_BORINGSSL_BUILD_SHA256_CHECKSUM
    a343962da2fbb10d8fa2cd9a2832839a23045a197c0ff306dc0fa0abb85759b3)
string(CONCAT VELOX_BORINGSSL_SOURCE_URL
              "https://github.com/google/boringssl/archive/refs/tags/"
              "${VELOX_BORINGSSL_BUILD_VERSION}.tar.gz")

resolve_dependency_url(BORINGSSL)

message(STATUS "Building BoringSSL from source")

FetchContent_Declare(
  boringssl
  URL ${VELOX_BORINGSSL_SOURCE_URL}
  URL_HASH ${VELOX_BORINGSSL_BUILD_SHA256_CHECKSUM}
  OVERRIDE_FIND_PACKAGE EXCLUDE_FROM_ALL SYSTEM)

set(CMAKE_BUILD_TYPE Release)
FetchContent_MakeAvailable(boringssl)
add_library(boringssl::ssl ALIAS ssl)
add_library(boringssl::crypto ALIAS crypto)
