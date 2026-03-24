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


set(COMMON_TEST_SRC
        test_cast
        test_checksum
        test_file_system
        test_caching_file_system_wrapper
        test_hyperlog
        test_lru_cache
        test_numeric_cast
        test_parse_logical_type
        test_utf
        test_storage_fuzz
        test_strftime
        test_string_util
)

foreach (test IN LISTS COMMON_TEST_SRC)
    kmcmake_cc_test(
            NAME ${test}
            MODULE capi
            SOURCES
            ${test}.cc
            CXXOPTS ${KMCMAKE_CXX_OPTIONS}
            LINKS
            goose::goose_testing_main_static
            goose::goose_testing_static
            goose::goose_static
            ${KMCMAKE_DEPS_LINK}
    )
endforeach ()
