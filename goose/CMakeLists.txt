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

if (SHADOW_FORBIDDEN_FUNCTIONS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include 'goose/common/shadow_forbidden_functions.h'")
endif ()

##########################################################
# db files
file(GLOB_RECURSE CATALOG_SRC "catalog/*.cc")
file(GLOB_RECURSE COMMON_SRC "common/*.cc")
file(GLOB_RECURSE execution_SRC "execution/*.cc")
file(GLOB_RECURSE function_SRC "function/*.cc")
file(GLOB_RECURSE imdb_SRC "imdb/*.cc")
file(GLOB_RECURSE logging_SRC "logging/*.cc")
file(GLOB_RECURSE main_SRC "main/*.cc")
file(GLOB_RECURSE optimizer_SRC "optimizer/*.cc")
file(GLOB_RECURSE parallel_SRC "parallel/*.cc")
file(GLOB_RECURSE parser_SRC "parser/*.cc")
file(GLOB_RECURSE planner_SRC "planner/*.cc")
file(GLOB_RECURSE storage_SRC "storage/*.cc")
file(GLOB_RECURSE transaction_SRC "transaction/*.cc")
file(GLOB_RECURSE utility_SRC "utility/*.cc")
file(GLOB_RECURSE verification_SRC "verification/*.cc")

include_directories(include)

file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp" "include/*.inc")

set(LIB_SRC
        ${CATALOG_SRC}
        ${COMMON_SRC}
        ${execution_SRC}
        ${function_SRC}
        ${imdb_SRC}
        ${logging_SRC}
        ${main_SRC}
        ${optimizer_SRC}
        ${parallel_SRC}
        ${parser_SRC}
        ${planner_SRC}
        ${storage_SRC}
        ${transaction_SRC}
        ${utility_SRC}
        ${verification_SRC}
)

#
if (STATIC_LOAD_CORE_FUNCTION)
    file(GLOB_RECURSE core_function_SRC "extension/core_functions/*.cc")
    list(APPEND LIB_SRC  ${core_function_SRC})
else ()
    add_subdirectory(extension/core_functions)
endif ()

if (STATIC_LOAD_JSON)
    file(GLOB_RECURSE json_SRC "extension/json/*.cc")
    list(APPEND LIB_SRC  ${json_SRC})
else ()
    add_subdirectory(extension/json)
endif ()

if (STATIC_LOAD_PARQUET)
    file(GLOB_RECURSE parquet_SRC "extension/parquet/*.cc")
    list(APPEND LIB_SRC  ${parquet_SRC})
else ()
    add_subdirectory(extension/parquet)
endif ()

if (STATIC_LOAD_ICU)
    file(GLOB_RECURSE icu_SRC "extension/icu/*.cc")
    list(APPEND LIB_SRC  ${icu_SRC})
else ()
    add_subdirectory(extension/icu)
endif ()

kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME goose
        SOURCES
        ${LIB_SRC}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        INCLUDES
        ${EXT_INCLUDES}
        SHARE
            ${ENABLE_SHARE_LIB}
        PLINKS
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
        UNITY
)

file(GLOB_RECURSE shell_SRC "shell/*.cc")
list(REMOVE_ITEM ${PROJECT_SOURCE_DIR}/goose/shell/shell.cc)
if(WIN32 AND NOT MINGW)
    string(TIMESTAMP GOOSE_COPYRIGHT_YEAR "%Y")
    ensure_variable_is_number(GOOSE_MAJOR_VERSION RC_MAJOR_VERSION)
    ensure_variable_is_number(GOOSE_MINOR_VERSION RC_MINOR_VERSION)
    ensure_variable_is_number(GOOSE_PATCH_VERSION RC_PATCH_VERSION)
    ensure_variable_is_number(GOOSE_DEV_ITERATION RC_DEV_ITERATION)

    set(CMAKE_RC_FLAGS
            "${CMAKE_RC_FLAGS} -D GOOSE_VERSION=\"${GOOSE_VERSION}\"")
    set(CMAKE_RC_FLAGS
            "${CMAKE_RC_FLAGS} -D GOOSE_MAJOR_VERSION=\"${RC_MAJOR_VERSION}\"")
    set(CMAKE_RC_FLAGS
            "${CMAKE_RC_FLAGS} -D GOOSE_MINOR_VERSION=\"${RC_MINOR_VERSION}\"")
    set(CMAKE_RC_FLAGS
            "${CMAKE_RC_FLAGS} -D GOOSE_PATCH_VERSION=\"${RC_PATCH_VERSION}\"")
    set(CMAKE_RC_FLAGS
            "${CMAKE_RC_FLAGS} -D GOOSE_DEV_ITERATION=\"${RC_DEV_ITERATION}\"")
    set(CMAKE_RC_FLAGS
            "${CMAKE_RC_FLAGS} -D GOOSE_COPYRIGHT_YEAR=\"${GOOSE_COPYRIGHT_YEAR}\"")
    set(${shell_SRC} shell/rc/goose.rc)
endif()
kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME goose_shell
        SOURCES
        ${shell_SRC}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        goose::goose_static
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)



file(GLOB_RECURSE testing_SRC "testing/*.cc")
kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME goose_testing
        SOURCES
        ${testing_SRC}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        SHARE
        ${ENABLE_SHARE_LIB}
        PLINKS
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)

file(GLOB_RECURSE benchmark_SRC "benchmark/*.cc")
kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME goose_benchmark
        SOURCES
        ${benchmark_SRC}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        SHARE
        ${ENABLE_SHARE_LIB}
        PLINKS
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)

#######################################################
# other extensions
add_subdirectory(extension/autocomplete)
add_subdirectory(extension/vss)
add_subdirectory(extension/httpfs)
add_subdirectory(extension/tpch)
add_subdirectory(extension/tpcds)

kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME goose
        SOURCES
        shell/shell.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        goose::goose_shell_static
        goose::goose_static
        goose::goose_autocomplete
        goose::goose_vss
        goose::goose_httpfs
        ${KMCMAKE_DEPS_LINK}
        ${HTTPFS_DEPS_LINK}
        LINKOPTS
        ${KMCMAKE_STATIC_BIN_OPTION}
        PUBLIC
)

##############################################
# header installing
##############################################
