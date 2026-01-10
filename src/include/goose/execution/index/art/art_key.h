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
#include <goose/common/exception.h>
#include <goose/common/radix.h>
#include <goose/common/types/string_type.h>
#include <goose/common/types/value.h>
#include <goose/storage/arena_allocator.h>

namespace goose {

class ARTKey {
public:
	ARTKey();
	ARTKey(data_ptr_t data, idx_t len);
	ARTKey(ArenaAllocator &allocator, idx_t len);

	idx_t len;
	data_ptr_t data;

public:
	template <class T>
	static inline ARTKey CreateARTKey(ArenaAllocator &allocator, T value) {
		auto data = ARTKey::CreateData<T>(allocator, value);
		return ARTKey(data, sizeof(value));
	}

	template <class T>
	static inline ARTKey CreateARTKey(ArenaAllocator &allocator, Value &value) {
		return CreateARTKey(allocator, value.GetValueUnsafe<T>());
	}

	template <class T>
	static inline void CreateARTKey(ArenaAllocator &allocator, ARTKey &key, T value) {
		key.data = ARTKey::CreateData<T>(allocator, value);
		key.len = sizeof(value);
	}

	template <class T>
	static inline void CreateARTKey(ArenaAllocator &allocator, ARTKey &key, Value value) {
		key.data = ARTKey::CreateData<T>(allocator, value.GetValueUnsafe<T>());
		key.len = sizeof(value);
	}

	static ARTKey CreateKey(ArenaAllocator &allocator, PhysicalType type, Value &value);

public:
	data_t &operator[](idx_t i) {
		return data[i];
	}
	const data_t &operator[](idx_t i) const {
		return data[i];
	}
	bool operator>(const ARTKey &key) const;
	bool operator>=(const ARTKey &key) const;
	bool operator==(const ARTKey &key) const;

	inline bool ByteMatches(const ARTKey &other, idx_t depth) const {
		return data[depth] == other[depth];
	}
	inline bool Empty() const {
		return len == 0;
	}

	void Concat(ArenaAllocator &allocator, const ARTKey &other);
	row_t GetRowId() const;
	idx_t GetMismatchPos(const ARTKey &other, const idx_t start) const;
	void VerifyKeyLength(const idx_t max_len) const;

private:
	template <class T>
	static inline data_ptr_t CreateData(ArenaAllocator &allocator, T value) {
		auto data = allocator.Allocate(sizeof(value));
		Radix::EncodeData<T>(data, value);
		return data;
	}
};

template <>
ARTKey ARTKey::CreateARTKey(ArenaAllocator &allocator, string_t value);
template <>
ARTKey ARTKey::CreateARTKey(ArenaAllocator &allocator, const char *value);
template <>
void ARTKey::CreateARTKey(ArenaAllocator &allocator, ARTKey &key, string_t value);

} // namespace goose
