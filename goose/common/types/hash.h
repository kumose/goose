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

#include <goose/common/common.h>
#include <goose/common/types.h>
#include <goose/common/types/datetime.h>

namespace goose {

struct string_t;
struct interval_t; // NOLINT

//! Combine two hashes by XORing them
inline hash_t CombineHash(hash_t left, hash_t right) {
	return left ^ right;
}

#ifdef GOOSE_HASH_ZERO
template <class T>
hash_t Hash(T value) {
	return 0;
}

GOOSE_API hash_t Hash(const char *val, size_t size);
GOOSE_API hash_t Hash(uint8_t *val, size_t size);
#else
//! Efficient hash function that maximizes the avalanche effect and minimizes bias
//! See: https://nullprogram.com/blog/2018/07/31/
inline hash_t MurmurHash64(uint64_t x) {
	x ^= x >> 32;
	x *= 0xd6e8feb86659fd93U;
	x ^= x >> 32;
	x *= 0xd6e8feb86659fd93U;
	x ^= x >> 32;
	return x;
}

inline hash_t MurmurHash32(uint32_t x) {
	return MurmurHash64(x);
}

template <class T>
hash_t Hash(T value) {
	return MurmurHash32(static_cast<uint32_t>(value));
}

template <>
GOOSE_API inline hash_t Hash(uint64_t val) {
	return MurmurHash64(val);
}
template <>
GOOSE_API inline hash_t Hash(int64_t val) {
	return MurmurHash64(static_cast<uint64_t>(val));
}
template <>
GOOSE_API hash_t Hash(hugeint_t val);
template <>
GOOSE_API hash_t Hash(uhugeint_t val);
template <>
GOOSE_API hash_t Hash(float val);
template <>
GOOSE_API hash_t Hash(double val);
template <>
GOOSE_API hash_t Hash(const char *val);
template <>
GOOSE_API hash_t Hash(char *val);
template <>
GOOSE_API hash_t Hash(string_t val);
template <>
GOOSE_API hash_t Hash(interval_t val);
template <>
GOOSE_API hash_t Hash(dtime_tz_t val);
GOOSE_API hash_t Hash(const char *val, size_t size);
GOOSE_API hash_t Hash(uint8_t *val, size_t size);
#endif

} // namespace goose
