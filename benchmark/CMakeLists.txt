# Copyright (C) Kumo inc. and its affiliates.
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
#

#[[kmcmake_cc_test(
        NAME custom_comparator_test
        SOURCES custom_comparator_test.cc
        DEPS turbo::turbo ${KMCMAKE_DEPS_LINK} ${GTEST_LIB} ${GTEST_MAIN_LIB}
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
)]]
#[[
kmcmake_cc_test(
        NAME foo_test
        SOURCES foo_test.cc
        DEPS ${PROJECT}::foo ${KMCMAKE_DEPS_LINK} gtest gtest_main
        COPTS ${KMCMAKE_CXX_OPTIONS}
)

kmcmake_cc_test(
        NAME foo_doctest
        SOURCES foo_doctest.cc
        DEPS ${PROJECT}::foo ${KMCMAKE_DEPS_LINK} doctest
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
)
]]

include_directories(${PROJECT_SOURCE_DIR}/goose/include)
set(BENCHMARK_ROOT_DIRECTORY
        ""
        CACHE
        STRING
        "The unittest root directory (defaults to the project source directory)")
if(DEFINED BENCHMARK_ROOT_DIRECTORY AND NOT BENCHMARK_ROOT_DIRECTORY STREQUAL "")
  set(GOOSE_ROOT_DIRECTORY "${BENCHMARK_ROOT_DIRECTORY}")
else()
  set(GOOSE_ROOT_DIRECTORY "${PROJECT_SOURCE_DIR}")
endif()

configure_file(${PROJECT_SOURCE_DIR}/benchmark/config.h.in ${PROJECT_SOURCE_DIR}/benchmark/config.h @ONLY)

kmcmake_cc_bm(
        NAME all_benchmark
        MODULE scripts
        SOURCES
        goose_benchmark_main.cc
        micro/append.cc
        micro/append_mix.cc
        micro/bulkupdate.cc
        micro/cast.cc
        micro/in.cc
        micro/storage.cc
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
        LINKS
        goose::goose_benchmark_static
        goose::goose_testing_static
        goose::goose_static
        goose::goose_vss
        goose::goose_tpch
        goose::goose_tpcds
        ${KMCMAKE_DEPS_LINK}
)
