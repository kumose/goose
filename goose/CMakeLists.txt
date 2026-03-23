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
###########################################################################
# below are examples, after initialization, modify it to your own.
###########################################################################
###kmcmake_example

option(SHADOW_FORBIDDEN_FUNCTIONS "Compile time test on usage of deprecated functions" FALSE)
if (SHADOW_FORBIDDEN_FUNCTIONS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include 'goose/common/shadow_forbidden_functions.h'")
endif ()

file(GLOB_RECURSE ALL_SRC "*.cc")
list(REMOVE_ITEM ALL_SRC ${PROJECT_SOURCE_DIR}/goose/utility/utf8proc_data.cc)
kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME goose
        SOURCES
        ${ALL_SRC}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)

###kmcmake_example