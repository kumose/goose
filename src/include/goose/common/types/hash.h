// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

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
