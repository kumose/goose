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

kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME goose_testing_main
        SOURCES
        goose_testing_main.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${KMCMAKE_DEPS_LINK}
)

add_subdirectory(api)
add_subdirectory(appender)
add_subdirectory(arrow)
add_subdirectory(catalog)
add_subdirectory(common)
add_subdirectory(logging)
add_subdirectory(mbedtls)
add_subdirectory(optimizer)
add_subdirectory(ossfuzz)
add_subdirectory(parallel_csv)
add_subdirectory(persistence)
add_subdirectory(secrets)
add_subdirectory(serialize)
add_subdirectory(sql)


