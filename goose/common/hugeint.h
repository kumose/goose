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
//


#pragma once

#include <goose/common/winapi.h>
#include <goose/common/string.h>
#include <stdint.h>
#include <goose/common/typedefs.h>

namespace goose {
    // Forward declaration to allow conversion between hugeint and uhugeint
    struct uhugeint_t; // NOLINT: use numeric casing

    struct hugeint_t { // NOLINT: use numeric casing
    public:
        uint64_t lower;
        int64_t upper;

    public:
        hugeint_t() = default;

        GOOSE_API hugeint_t(int64_t value); // NOLINT: Allow implicit conversion from `int64_t`
        constexpr hugeint_t(int64_t upper, uint64_t lower) : lower(lower), upper(upper) {
        }

        constexpr hugeint_t(const hugeint_t &rhs) = default;

        constexpr hugeint_t(hugeint_t &&rhs) = default;

        hugeint_t &operator=(const hugeint_t &rhs) = default;

        hugeint_t &operator=(hugeint_t &&rhs) = default;

        GOOSE_API string ToString() const;

        // comparison operators
        GOOSE_API bool operator==(const hugeint_t &rhs) const;

        GOOSE_API bool operator!=(const hugeint_t &rhs) const;

        GOOSE_API bool operator<=(const hugeint_t &rhs) const;

        GOOSE_API bool operator<(const hugeint_t &rhs) const;

        GOOSE_API bool operator>(const hugeint_t &rhs) const;

        GOOSE_API bool operator>=(const hugeint_t &rhs) const;

        // arithmetic operators
        GOOSE_API hugeint_t operator+(const hugeint_t &rhs) const;

        GOOSE_API hugeint_t operator-(const hugeint_t &rhs) const;

        GOOSE_API hugeint_t operator*(const hugeint_t &rhs) const;

        GOOSE_API hugeint_t operator/(const hugeint_t &rhs) const;

        GOOSE_API hugeint_t operator%(const hugeint_t &rhs) const;

        GOOSE_API hugeint_t operator-() const;

        // bitwise operators
        GOOSE_API hugeint_t operator>>(const hugeint_t &rhs) const;

        GOOSE_API hugeint_t operator<<(const hugeint_t &rhs) const;

        GOOSE_API hugeint_t operator&(const hugeint_t &rhs) const;

        GOOSE_API hugeint_t operator|(const hugeint_t &rhs) const;

        GOOSE_API hugeint_t operator^(const hugeint_t &rhs) const;

        GOOSE_API hugeint_t operator~() const;

        // in-place operators
        GOOSE_API hugeint_t &operator+=(const hugeint_t &rhs);

        GOOSE_API hugeint_t &operator-=(const hugeint_t &rhs);

        GOOSE_API hugeint_t &operator*=(const hugeint_t &rhs);

        GOOSE_API hugeint_t &operator/=(const hugeint_t &rhs);

        GOOSE_API hugeint_t &operator%=(const hugeint_t &rhs);

        GOOSE_API hugeint_t &operator>>=(const hugeint_t &rhs);

        GOOSE_API hugeint_t &operator<<=(const hugeint_t &rhs);

        GOOSE_API hugeint_t &operator&=(const hugeint_t &rhs);

        GOOSE_API hugeint_t &operator|=(const hugeint_t &rhs);

        GOOSE_API hugeint_t &operator^=(const hugeint_t &rhs);

        // boolean operators
        GOOSE_API explicit operator bool() const;

        GOOSE_API bool operator!() const;

        // cast operators -- doesn't check bounds/overflow/underflow
        GOOSE_API explicit operator uint8_t() const;

        GOOSE_API explicit operator uint16_t() const;

        GOOSE_API explicit operator uint32_t() const;

        GOOSE_API explicit operator uint64_t() const;

        GOOSE_API explicit operator int8_t() const;

        GOOSE_API explicit operator int16_t() const;

        GOOSE_API explicit operator int32_t() const;

        GOOSE_API explicit operator int64_t() const;

        GOOSE_API explicit operator uhugeint_t() const;
    };
} // namespace goose

namespace std {
    template<>
    struct hash<goose::hugeint_t> {
        size_t operator()(const goose::hugeint_t &val) const {
            using std::hash;
            return hash<int64_t>{}(val.upper) ^ hash<uint64_t>{}(val.lower);
        }
    };
} // namespace std
