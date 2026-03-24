// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

// project name version
#define GOOSE_VERSION_MAJOR 0
#define GOOSE_VERSION_MINOR 0
#define GOOSE_VERSION_PATCH 5
#define GOOSE_VERSION ((PROJECT_VERSION_MAJOR * 1000) + PROJECT_VERSION_MINOR) * 1000 + PROJECT_VERSION_PATCH

#define GOOSE_VERSION_STRING "0.0.5"

// build system
#define GOOSE_BUILD_SYSTEM "ubuntu 20.04.6 lts"

// build system version
#define GOOSE_BUILD_SYSTEM_VERSION "20.04"


// compiler gnu or clang
#define GOOSE_CXX_COMPILER_ID "GNU"

// compiler version
#define GOOSE_CXX_COMPILER_VERSION "10.5.0"

// cmake cxx compiler flags
#define GOOSE_CMAKE_CXX_COMPILER_FLAGS ""

// user defined cxx compiler flags
#define GOOSE_CXX_COMPILER_FLAGS "-Wall;-Wextra;-Wno-cast-qual;-Wconversion-null;-Wformat-security;-Woverlength-strings;-Wpointer-arith;-Wno-undef;-Wunused-local-typedefs;-Wunused-result;-Wvarargs;-Wno-attributes;-Wno-implicit-fallthrough;-Wno-unused-parameter;-Wno-unused-function;-Wwrite-strings;-Wclass-memaccess;-Wno-sign-compare;-DNOMINMAX;-msse;-msse2;-msse3;-mssse3;-msse4.1;-msse4;-msse4.2;-mpopcnt;-mlzcnt;-mavx;-mavx2;-mfma;-maes"

// cxx standard
#define GOOSE_CXX_STANDARD "17"

// build type
#define GOOSE_BUILD_TYPE_STRING "RELEASE"

// build type
#define GOOSE_BUILD_RELEASE

// build type
#if defined(GOOSE_BUILD_DEBUG)
    #define IS_GOOSE_BUILD_TYPE_DEBUG 1
#else
    #define IS_GOOSE_BUILD_TYPE_DEBUG 0
#endif

////////////////////////////////////////////////////////////////////////////////
/// simd region

#define KMCMAKE_SIMD_LEVEL_NONE 0
#define KMCMAKE_SIMD_LEVEL_SSE 1
#define KMCMAKE_SIMD_LEVEL_AVX 1
#define KMCMAKE_SIMD_LEVEL_AVX2 1
#define KMCMAKE_SIMD_LEVEL_BMI 0
#define KMCMAKE_SIMD_LEVEL_BMI2 0
#define KMCMAKE_SIMD_LEVEL_FMA 1
#define KMCMAKE_SIMD_LEVEL_MOVBE 0

///////////////////////////////////////////////////////////////////////////////////
/// git and version region

////////////////////////////////////////////////////////////////////////////////
/// Git Version Information
////////////////////////////////////////////////////////////////////////////////
// Full Git commit hash (e.g., "a1b2c3d4e5f67890abcdef1234567890abcdef12")
#define GOOSE_GIT_COMMIT_HASH "ebca15174a7466ac077a1f8676aec818a380f0cf"
// Short Git commit hash (e.g., "a1b2c3d")
#define GOOSE_GIT_COMMIT_SHORT_HASH "ebca1517"
// Git dirty flag (0 = clean working tree, 1 = uncommitted changes)
#define GOOSE_GIT_IS_DIRTY 1
// Combined Git version string (e.g., "0.6.0-a1b2c3d" or "0.6.0-a1b2c3d-dirty")
#define GOOSE_GIT_VERSION_STRING "v0.5.5-ebca1517-dirty"

#define GOOSE_CUSTOM_PLATFORM_ENABLE 0

#if GOOSE_CUSTOM_PLATFORM_ENABLE
#define GOOSE_CUSTOM_PLATFORM ""
#endif  // GOOSE_CUSTOM_PLATFORM_ENABLE

#define GOOSE_EXTENSION_AUTOLOAD_DEFAULT 0

#define GOOSE_EXTENSION_AUTOINSTALL_DEFAULT 0

#define GOOSE_EXTENSION_DIRECTORIES "~/.goose/extensions"

/* #undef CRASH_ON_ASSERT */


#ifdef CRASH_ON_ASSERT
   #define GOOSE_CRASH_ON_ASSERT
#endif

/* #undef DISABLE_STR_INLINE */
#ifdef CRASH_ON_ASSERT
   #define GOOSE_DEBUG_NO_INLINE
#endif

/* #undef FORCE_QUERY_LOG */
#ifdef FORCE_QUERY_LOG
#define GOOSE_FORCE_QUERY_LOG 1
#endif


/* #undef WASM_LOADABLE_EXTENSIONS */
/* #undef UNSAFE_NUMERIC_CAST */

/* #undef DISABLE_MEMORY_SAFETY */

#ifdef DISABLE_MEMORY_SAFETY
#define GOOSE_DEBUG_NO_SAFETY
#endif

/* #undef DISABLE_ASSERTIONS */
/* #undef DESTROY_UNPINNED_BLOCKS */
#ifdef DESTROY_UNPINNED_BLOCKS
#define GOOSE_DEBUG_DESTROY_BLOCKS
#endif

/* #undef FORCE_ASYNC_SINK_SOURCE */
#ifdef FORCE_ASYNC_SINK_SOURCE
#define GOOSE_DEBUG_ASYNC_SINK_SOURCE
#endif

/* #undef RUN_SLOW_VERIFIERS */
#ifdef RUN_SLOW_VERIFIERS
#define GOOSE_RUN_SLOW_VERIFIERS
#endif


/* #undef ALTERNATIVE_VERIFY */
#ifdef ALTERNATIVE_VERIFY
#define GOOSE_ALTERNATIVE_VERIFY
#endif

/* #undef DISABLE_POINTER_SALT */
#ifdef DISABLE_POINTER_SALT
#define GOOSE_DISABLE_POINTER_SALT
#endif

/* #undef HASH_ZERO */
#ifdef HASH_ZERO
#define GOOSE_HASH_ZERO
#endif



/* #undef LATEST_STORAGE */
#ifdef LATEST_STORAGE
#define GOOSE_LATEST_STORAGE
#endif

/* #undef DEBUG_ALLOCATION */
#ifdef DEBUG_ALLOCATION
#define GOOSE_DEBUG_ALLOCATION
#endif

/* #undef DEBUG_MOVE */
#ifdef DEBUG_MOVE
#define GOOSE_DEBUG_MOVE
#endif

/* #undef CLANG_TIDY */
#ifdef CLANG_TIDY
#define GOOSE_CLANG_TIDY
#endif

/* #undef SMALLER_BINARY */
#ifdef SMALLER_BINARY
#define GOOSE_SMALLER_BINARY
#endif

/* #undef FORCE_ASSERT */
#ifdef FORCE_ASSERT
#define GOOSE_FORCE_ASSERT
#endif

/* #undef STANDARD_VECTOR_SIZE_ENABLE */
#ifdef STANDARD_VECTOR_SIZE_ENABLE
#define STANDARD_VECTOR_SIZE 
#endif


/* #undef DEBUG_STACKTRACE */
#ifdef DEBUG_STACKTRACE
#define GOOSE_DEBUG_STACKTRACE
#endif

/* #undef DISABLE_EXTENSION_LOAD */
#ifdef DISABLE_EXTENSION_LOAD
#define GOOSE_DISABLE_EXTENSION_LOAD
#endif

#define HAVE_LINENOISE
#define SQLITE_SHELL_UTF8
#ifdef SQLITE_SHELL_UTF8
#define SQLITE_SHELL_IS_UTF8 1
#else
#define SQLITE_SHELL_IS_UTF8 0
#endif

/// enable static auto load core function module
#define STATIC_LOAD_CORE_FUNCTION

/// enable static auto load json module
#define STATIC_LOAD_JSON

/// enable static auto load parquet module
#define STATIC_LOAD_PARQUET

/// enable static auto load icu module
#define STATIC_LOAD_ICU

