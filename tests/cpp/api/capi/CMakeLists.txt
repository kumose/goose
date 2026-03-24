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
        SOURCES custom_comparator_test
        DEPS turbo::turbo ${KMCMAKE_DEPS_LINK} ${GTEST_LIB} ${GTEST_MAIN_LIB}
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
)]]
#[[
kmcmake_cc_test(
        NAME foo_test
        SOURCES foo_test
        DEPS ${PROJECT}::foo ${KMCMAKE_DEPS_LINK} gtest gtest_main
        COPTS ${KMCMAKE_CXX_OPTIONS}
)

kmcmake_cc_test(
        NAME foo_doctest
        SOURCES foo_doctest
        DEPS ${PROJECT}::foo ${KMCMAKE_DEPS_LINK} doctest
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
)
]]

set(C_TEST_SRC
        capi_aggregate_functions
        capi_catalog
        capi_config_options
        capi_copy_function
        capi_custom_type
        capi_file_system
        capi_scalar_functions
        capi_table_functions
        test_capi
        test_capi_any_invalid_type
        test_capi_append_data_chunk
        test_starting_database
        test_capi_appender
        test_capi_arrow
        test_capi_data_chunk
        test_capi_extract
        test_capi_instance_cache
        test_capi_logging
        test_capi_pending
        test_capi_prepared
        test_capi_profiling
        test_capi_website
        test_capi_complex_types
        test_capi_to_decimal
        test_capi_values
        test_capi_vector
        test_capi_replacement_scan
        test_capi_streaming
        test_capi_table_description
        test_without_disabled_functions
)

foreach (test IN LISTS C_TEST_SRC)
    kmcmake_cc_test(
            NAME ${test}
            MODULE capi
            SOURCES
            ${test}.cc
            CXXOPTS ${KMCMAKE_CXX_OPTIONS}
            LINKS
            goose::goose_testing_static
            goose::goose_testing_main_static
            goose::goose_static
            ${KMCMAKE_DEPS_LINK}
    )
endforeach ()