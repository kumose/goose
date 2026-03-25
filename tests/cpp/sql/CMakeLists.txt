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


set(SQL_TEST_SRC
        concurrent_attach_detach
        concurrent_checkpoint
        filter_cache
        multi_statement_parsing
        table_bind_replace
        table_in_out
        test_art_index
        test_art_keys
        test_buffer_manager
        test_checksum
        test_concurrent_dependencies
        test_concurrent_index
        test_concurrent_prepared
        test_concurrent_sequence
        test_concurrentappend
        test_concurrentdelete
        test_concurrentupdate
        test_database_size
        test_default_catalog
        test_storage
        wal_torn_write
)

foreach (test IN LISTS SQL_TEST_SRC)
    kmcmake_cc_test(
            NAME ${test}
            MODULE sql
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
