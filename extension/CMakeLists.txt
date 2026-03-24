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

set(EXT_INCLUDES)
message(STATUS "extensions:")


if (STATIC_LOAD_CORE_FUNCTION)
    add_subdirectory(core_functions)
    list(APPEND GOOSE_BUILTIN_EXTENSION goose::goose_core_functions)
    set(GOOSE_BUILTIN_EXTENSION ${GOOSE_BUILTIN_EXTENSION} PARENT_SCOPE)
    set(EXT_INCLUDES ${EXT_INCLUDES} ${PROJECT_SOURCE_DIR}/extension/core_functions/include PARENT_SCOPE)
endif ()


if (STATIC_LOAD_PARQUET)
    add_subdirectory(parquet)
    list(APPEND GOOSE_BUILTIN_EXTENSION goose::goose_parquet)
    set(GOOSE_BUILTIN_EXTENSION ${GOOSE_BUILTIN_EXTENSION} PARENT_SCOPE)
    set(EXT_INCLUDES ${EXT_INCLUDES} ${PROJECT_SOURCE_DIR}/extension/parquet/include PARENT_SCOPE)
endif ()

if (STATIC_LOAD_JSON)
    add_subdirectory(json)
    list(APPEND GOOSE_BUILTIN_EXTENSION goose::goose_json)
    set(GOOSE_BUILTIN_EXTENSION ${GOOSE_BUILTIN_EXTENSION} PARENT_SCOPE)
    set(EXT_INCLUDES ${EXT_INCLUDES} ${PROJECT_SOURCE_DIR}/extension/json/include PARENT_SCOPE)
endif ()

if (STATIC_LOAD_ICU)
    add_subdirectory(icu)
    list(APPEND GOOSE_BUILTIN_EXTENSION goose::goose_icu)
    set(GOOSE_BUILTIN_EXTENSION ${GOOSE_BUILTIN_EXTENSION} PARENT_SCOPE)
    set(EXT_INCLUDES ${EXT_INCLUDES} ${PROJECT_SOURCE_DIR}/extension/icu/include PARENT_SCOPE)
endif ()

#[[
message(STATUS "${GOOSE_BUILTIN_EXTENSION}")
kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME static_extension_loader
        SOURCES
        loader/static_extension_loader.cc
        INCLUDES
        ${EXT_INCLUDES}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        ${GOOSE_BUILTIN_EXTENSION}
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)
]]