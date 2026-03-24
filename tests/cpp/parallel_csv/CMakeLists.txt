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


set(PC_TEST_SRC
        test_parallel_csv
)

foreach (test IN LISTS PC_TEST_SRC)
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
