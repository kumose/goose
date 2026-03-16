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

#include <goose/common/exception.h>
#include <goose/common/types-import.h>
#include <goose/common/memory_safety.h>

namespace goose {

template <class DATA_TYPE>
class array_ptr_iterator { // NOLINT: match std naming style
public:
	array_ptr_iterator(DATA_TYPE *ptr, idx_t index, idx_t size) : ptr(ptr), index(index), size(size) {
	}

public:
	array_ptr_iterator<DATA_TYPE> &operator++() {
		index++;
		if (index > size) {
			index = size;
		}
		return *this;
	}
	bool operator!=(const array_ptr_iterator<DATA_TYPE> &other) const {
		return ptr != other.ptr || index != other.index || size != other.size;
	}
	DATA_TYPE &operator*() const {
		if (TURBO_UNLIKELY(index >= size)) {
			throw InternalException("array_ptr iterator dereferenced while iterator is out of range");
		}
		return ptr[index];
	}

private:
	DATA_TYPE *ptr;
	idx_t index;
	idx_t size;
};

//! array_ptr is a non-owning (optionally) bounds-checked pointer to an array
template <class DATA_TYPE, bool SAFE = true>
class array_ptr { // NOLINT: match std naming style
public:
	using iterator_type = array_ptr_iterator<DATA_TYPE>;

private:
	static inline void AssertNotNull(const bool null) {
#if defined(GOOSE_DEBUG_NO_SAFETY) || defined(GOOSE_CLANG_TIDY)
		return;
#else
		if (TURBO_UNLIKELY(null)) {
			throw goose::InternalException("Attempted to construct an array_ptr from a NULL pointer");
		}
#endif
	}

	static inline void AssertIndexInBounds(idx_t index, idx_t size) {
#if defined(GOOSE_DEBUG_NO_SAFETY) || defined(GOOSE_CLANG_TIDY)
		return;
#else
		if (TURBO_UNLIKELY(index >= size)) {
			throw InternalException("Attempted to access index %ld within array_ptr of size %ld", index, size);
		}
#endif
	}

public:
	array_ptr(DATA_TYPE *ptr_p, idx_t count) : ptr(ptr_p), count(count) {
		if (MemorySafety<SAFE>::ENABLED) {
			AssertNotNull(!ptr);
		}
	}
	explicit array_ptr(DATA_TYPE &ref) : ptr(&ref), count(1) {
	}

	const DATA_TYPE &operator[](idx_t idx) const {
		if (MemorySafety<SAFE>::ENABLED) {
			AssertIndexInBounds(idx, count);
		}
		return ptr[idx];
	}

	DATA_TYPE &operator[](idx_t idx) {
		if (MemorySafety<SAFE>::ENABLED) {
			AssertIndexInBounds(idx, count);
		}
		return ptr[idx];
	}

	idx_t size() const { // NOLINT: match std naming style
		return count;
	}

	array_ptr_iterator<DATA_TYPE> begin() { // NOLINT: match std naming style
		return array_ptr_iterator<DATA_TYPE>(ptr, 0, count);
	}
	array_ptr_iterator<DATA_TYPE> begin() const { // NOLINT: match std naming style
		return array_ptr_iterator<const DATA_TYPE>(ptr, 0, count);
	}
	array_ptr_iterator<DATA_TYPE> cbegin() { // NOLINT: match std naming style
		return array_ptr_iterator<const DATA_TYPE>(ptr, 0, count);
	}
	array_ptr_iterator<DATA_TYPE> end() { // NOLINT: match std naming style
		return array_ptr_iterator<DATA_TYPE>(ptr, count, count);
	}
	array_ptr_iterator<DATA_TYPE> end() const { // NOLINT: match std naming style
		return array_ptr_iterator<const DATA_TYPE>(ptr, count, count);
	}
	array_ptr_iterator<DATA_TYPE> cend() { // NOLINT: match std naming style
		return array_ptr_iterator<const DATA_TYPE>(ptr, count, count);
	}

private:
	DATA_TYPE *ptr;
	idx_t count;
};

template <typename T>
using unsafe_array_ptr = array_ptr<T, false>;

} // namespace goose
