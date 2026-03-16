// Copyright (c) 2025 DuckDB.
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

#pragma once
#include <string>

// duplicated from string_util.h to avoid linking issues
#ifndef GOOSE_QUOTE_DEFINE
#define GOOSE_QUOTE_DEFINE_IMPL(x) #x
#define GOOSE_QUOTE_DEFINE(x)      GOOSE_QUOTE_DEFINE_IMPL(x)
#endif

#if defined(_WIN32) || defined(__APPLE__) || defined(__FreeBSD__)
#else
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#include <features.h>
#ifndef __USE_GNU
#define __MUSL__
#endif
#undef _GNU_SOURCE /* don't contaminate other includes unnecessarily */
#else
#include <features.h>
#ifndef __USE_GNU
#define __MUSL__
#endif
#endif
#endif

namespace goose {

std::string GoosePlatform() { // NOLINT: allow definition in header
#if defined(GOOSE_CUSTOM_PLATFORM)
	return GOOSE_QUOTE_DEFINE(GOOSE_CUSTOM_PLATFORM);
#else
#if defined(GOOSE_WASM_VERSION)
	// Goose-Wasm requires CUSTOM_PLATFORM to be defined
	static_assert(0, "GOOSE_WASM_VERSION should rely on CUSTOM_PLATFORM being provided");
#endif
	std::string os = "linux";
#if INTPTR_MAX == INT64_MAX
	std::string arch = "amd64";
#elif INTPTR_MAX == INT32_MAX
	std::string arch = "i686";
#else
#error Unknown pointer size or missing size macros!
#endif
	std::string postfix = "";

#ifdef _WIN32
	os = "windows";
#elif defined(__APPLE__)
	os = "osx";
#elif defined(__FreeBSD__)
	os = "freebsd";
#endif
#if defined(__aarch64__) || defined(__ARM_ARCH_ISA_A64)
	arch = "arm64";
#endif

#if defined(__MUSL__)
	if (os == "linux") {
		postfix = "_musl";
	}
#elif (!defined(__clang__) && defined(__GNUC__) && __GNUC__ < 5) ||                                                    \
    (defined(_GLIBCXX_USE_CXX11_ABI) && _GLIBCXX_USE_CXX11_ABI == 0)
#error                                                                                                                 \
    "Goose does not provide extensions for this (legacy) CXX ABI - Explicitly set GOOSE_PLATFORM (Makefile) / GOOSE_EXPLICIT_PLATFORM (CMake) to build anyway. "
#endif

#if defined(__ANDROID__)
	postfix = "_android";
#endif
#ifdef __MINGW32__
	postfix = "_mingw";
#endif
// this is used for the windows R builds which use `mingw` equivalent extensions
#ifdef GOOSE_PLATFORM_RTOOLS
	postfix = "_mingw";
#endif
	return os + "_" + arch + postfix;
#endif
}

} // namespace goose
