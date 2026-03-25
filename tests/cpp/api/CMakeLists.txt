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

set(C_TEST_SRC
        test_api
        test_appender_api
        test_bignum
        test_config
        test_custom_allocator
        test_dbdir
        test_extension_setting_autoload
        test_get_table_names
        test_insertion_order_preserving_map
        test_instance_cache
        test_lifecycle_hooks
        test_object_cache
        test_pending_query
        test_pending_with_parameters
        test_plan_serialization
        test_prepared_api
        test_query_profiler
        test_read_only
        test_relation_api
        test_reset
        test_results
        test_storage_extension_alias
        test_table_info
        test_threads
        #test_tpch_with_relations
        #test_tpch_with_streaming
        test_uuid
        test_windows_header_compatibility
        test_windows_unicode_path
)

foreach (test IN LISTS C_TEST_SRC)
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
add_subdirectory(adbc)
add_subdirectory(capi)
add_subdirectory(udf_function)
