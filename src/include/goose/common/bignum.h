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

#include <goose/common/winapi.h>
#include <goose/common/string.h>
#include <stdint.h>
#include <goose/common/types/string_type.h>
#include <goose/storage/arena_allocator.h>

namespace goose {

struct bignum_t {
	string_t data;

	bignum_t() : data() {
	}

	explicit bignum_t(const string_t &data) : data(data) {
	}

	bignum_t(const bignum_t &rhs) = default;
	bignum_t(bignum_t &&other) = default;
	bignum_t &operator=(const bignum_t &rhs) = default;
	bignum_t &operator=(bignum_t &&rhs) = default;

	void Print() const;
};

enum AbsoluteNumberComparison : uint8_t {
	// If number is equal
	EQUAL = 0,
	// If compared number is greater
	GREATER = 1,
	// If compared number is smaller
	SMALLER = 2,
};

struct BignumIntermediate {
	BignumIntermediate() : is_negative(false), size(0), data(nullptr) {};
	explicit BignumIntermediate(const bignum_t &value);
	BignumIntermediate(uint8_t *value, idx_t size);
	void Print() const;
	//! Information on the header
	bool is_negative;
	uint32_t size;
	//! The actual data
	data_ptr_t data;
	//! If the absolute number is  bigger than the absolute rhs
	//! 1 = true, 0 = equal, -1 = false
	AbsoluteNumberComparison IsAbsoluteBigger(const BignumIntermediate &rhs) const;
	//! Get the absolute value of a byte
	uint8_t GetAbsoluteByte(int64_t index) const;
	//! If the most significant bit of the first byte is set.
	bool IsMSBSet() const;
	//! Initializes our bignum to 0 and 1 byte
	void Initialize(ArenaAllocator &allocator);
	//! If necessary, we reallocate our intermediate to the next power of 2.
	void Reallocate(ArenaAllocator &allocator, idx_t min_size);
	static uint32_t GetStartDataPos(data_ptr_t data, idx_t size, bool is_negative);
	uint32_t GetStartDataPos() const;
	//! In case we have unnecessary extra 0's or 1's in our bignum we trim them
	static idx_t Trim(data_ptr_t data, uint32_t &size, bool is_negative);
	void Trim();
	//! Add a BignumIntermediate to another BignumIntermediate, equivalent of a +=
	void AddInPlace(ArenaAllocator &allocator, const BignumIntermediate &rhs);
	//! Adds two BignumIntermediates and returns a string_t result, equivalent of a +
	static string_t Add(Vector &result, const BignumIntermediate &lhs, const BignumIntermediate &rhs);
	//! Negates a value, e.g., -x
	string_t Negate(Vector &result_vector) const;
	void NegateInPlace();
	//! Exports to a bignum, either arena allocated
	bignum_t ToBignum(ArenaAllocator &allocator);
	//! Check if an over/underflow has occurred
	static bool OverOrUnderflow(data_ptr_t data, idx_t size, bool is_negative);
	bool OverOrUnderflow() const;
};

} // namespace goose
