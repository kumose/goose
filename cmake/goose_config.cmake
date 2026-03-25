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

set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MACOSX_RPATH 1)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if (GOOSE_EXPLICIT_PLATFORM)
    set(GOOSE_CUSTOM_PLATFORM_ENABLE 1)
    set(GOOSE_CUSTOM_PLATFORM="${GOOSE_EXPLICIT_PLATFORM}")
else ()
    set(GOOSE_CUSTOM_PLATFORM_ENABLE 0)
endif ()

option(ENABLE_EXTENSION_AUTOLOADING "Enable extension auto-loading by default." FALSE)

set(GOOSE_EXTENSION_AUTOLOAD_DEFAULT 0)
if (ENABLE_EXTENSION_AUTOLOADING)
    set(GOOSE_EXTENSION_AUTOLOAD_DEFAULT 1)
endif ()

option(ENABLE_EXTENSION_AUTOINSTALL "Enable extension auto-installing by default." FALSE)

set(GOOSE_EXTENSION_AUTOINSTALL_DEFAULT 0)
if (ENABLE_EXTENSION_AUTOINSTALL)
    set(GOOSE_EXTENSION_AUTOINSTALL_DEFAULT 1)
endif ()

set(EXTENSION_DIRECTORIES "~/.goose/extensions" CACHE STRING "Default extension directories (;-separated list on win, : on unix)")

string(REPLACE ";" "\;" EXT_DIR_ESCAPED "${EXTENSION_DIRECTORIES}")
set(GOOSE_EXTENSION_DIRECTORIES "${EXT_DIR_ESCAPED}")

option(CRASH_ON_ASSERT "Trigger a sigabort on an assert failing, instead of throwing an exception" FALSE)

option(DISABLE_STR_INLINE "Debug setting: disable inlining of strings" FALSE)


option(FORCE_QUERY_LOG "If enabled, all queries will be logged to the specified path" OFF)

option(WASM_LOADABLE_EXTENSIONS "WebAssembly build with loadable extensions." FALSE)
option(UNSAFE_NUMERIC_CAST "UNSAFE_NUMERIC_CAST" OFF)

option(DISABLE_MEMORY_SAFETY "Debug setting: disable memory access checks at runtime" FALSE)
option(DISABLE_ASSERTIONS "Debug setting: disable assertions" FALSE)
option(DESTROY_UNPINNED_BLOCKS "Debug setting: destroy unpinned buffer-managed blocks" FALSE)
option(FORCE_ASYNC_SINK_SOURCE "Debug setting: forces sinks/sources to block the first 2 times they're called" FALSE)
option(RUN_SLOW_VERIFIERS "Debug setting: enable a more extensive set of verifiers" FALSE)
option(ALTERNATIVE_VERIFY "Debug setting: use alternative verify mode" FALSE)
option(DISABLE_POINTER_SALT "Debug setting: verify correct results without pointer salt" FALSE)
option(HASH_ZERO "Debug setting: verify hash collision resolution by setting all hashes to 0" FALSE)
option(LATEST_STORAGE "" FALSE)
option(DEBUG_MOVE "Debug setting: Ensure std::move is being used" FALSE)
option(DEBUG_ALLOCATION "Debug setting: keep track of outstanding allocations to detect memory leaks" FALSE)
option(CLANG_TIDY "Enable build for clang-tidy, this disables all source files excluding the core database. This does not produce a working build." FALSE)
option(SMALLER_BINARY "Produce a smaller binary by trimming specialized code paths. This can negatively affect performance." FALSE)
option(FORCE_ASSERT "Enable checking of assertions, even in release mode" FALSE)
option(SHADOW_FORBIDDEN_FUNCTIONS "Compile time test on usage of deprecated functions" FALSE)

set(STANDARD_VECTOR_SIZE "" CACHE STRING "Set a custom STANDARD_VECTOR_SIZE at compile time")

function(is_number input_string return_var)
    if ("${input_string}" MATCHES "^[0-9]+$")
        set(${return_var} TRUE PARENT_SCOPE)
    else ()
        set(${return_var} FALSE PARENT_SCOPE)
    endif ()
endfunction()

if (DEFINED STANDARD_VECTOR_SIZE AND NOT STANDARD_VECTOR_SIZE STREQUAL "")
    is_number(${STANDARD_VECTOR_SIZE} is_number_result)
    if (is_number_result)
        set(STANDARD_VECTOR_SIZE_ENABLE ON)
        message(STATUS "STANDARD_VECTOR_SIZE is set to ${STANDARD_VECTOR_SIZE}")
    else ()
        message(FATAL_ERROR "STANDARD_VECTOR_SIZE must be a number, not ${STANDARD_VECTOR_SIZE}")
    endif ()
endif ()

option(DISABLE_THREADS "Disable support for multi-threading" FALSE)
option(DEBUG_STACKTRACE "Debug setting: print a stracktrace on asserts and when testing crashes" FALSE)
option(GENERATE_EXTENSION_ENTRIES "Build for generating extension_entries.h" FALSE)
option(DISABLE_EXTENSION_LOAD "Disable support for loading and installing extensions" FALSE)
option(HAVE_LINENOISE "enable linenoise" ON)
option(SQLITE_SHELL_UTF8 "shell using utf8" ON)

option(STATIC_LOAD_CORE_FUNCTION "enable static auto load core function module" ON)
option(STATIC_LOAD_JSON "enable static auto load json module" ON)
option(STATIC_LOAD_PARQUET "enable static auto load parquet module" ON)
option(STATIC_LOAD_ICU "enable static auto load icu module" ON)

set(GOOSE_BUILTIN_EXTENSION)
set(EXT_INCLUDES)
