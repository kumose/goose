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

#include <goose/common/assert.h>
#include <goose/common/typedefs.h>
#include <goose/common/types-import.h>
#include <goose/common/exception.h>
#include <goose/common/memory_safety.h>
#include <vector>

namespace goose {

template <class DATA_TYPE, bool SAFE = true, class ALLOCATOR = std::allocator<DATA_TYPE>>
class vector : public std::vector<DATA_TYPE, ALLOCATOR> { // NOLINT: matching name of std
public:
	using original = std::vector<DATA_TYPE, ALLOCATOR>;
	using original::original;
	using value_type = typename original::value_type;
	using allocator_type = typename original::allocator_type;
	using size_type = typename original::size_type;
	using difference_type = typename original::difference_type;
	using reference = typename original::reference;
	using const_reference = typename original::const_reference;
	using pointer = typename original::pointer;
	using const_pointer = typename original::const_pointer;
	using iterator = typename original::iterator;
	using const_iterator = typename original::const_iterator;
	using reverse_iterator = typename original::reverse_iterator;
	using const_reverse_iterator = typename original::const_reverse_iterator;

private:
	static inline void AssertIndexInBounds(idx_t index, idx_t size) {
#if defined(GOOSE_DEBUG_NO_SAFETY) || defined(GOOSE_CLANG_TIDY)
		return;
#else
		if (TURBO_UNLIKELY(index >= size)) {
			throw InternalException("Attempted to access index %ld within vector of size %ld", index, size);
		}
#endif
	}

public:
#ifdef GOOSE_CLANG_TIDY
	// This is necessary to tell clang-tidy that it reinitializes the variable after a move
	[[clang::reinitializes]]
#endif
	inline void
	clear() noexcept { // NOLINT: hiding on purpose
		original::clear();
	}

	// Because we create the other constructor, the implicitly created constructor
	// gets deleted, so we have to be explicit
	vector() = default;
	vector(original &&other) : original(std::move(other)) { // NOLINT: allow implicit conversion
	}
	template <bool INTERNAL_SAFE>
	vector(vector<DATA_TYPE, INTERNAL_SAFE> &&other) : original(std::move(other)) { // NOLINT: allow implicit conversion
	}

	template <bool INTERNAL_SAFE = false>
	inline typename original::reference get(typename original::size_type __n) { // NOLINT: hiding on purpose
		if (MemorySafety<INTERNAL_SAFE>::ENABLED) {
			AssertIndexInBounds(__n, original::size());
		}
		return original::operator[](__n);
	}

	template <bool INTERNAL_SAFE = false>
	inline typename original::const_reference get(typename original::size_type __n) const { // NOLINT: hiding on purpose
		if (MemorySafety<INTERNAL_SAFE>::ENABLED) {
			AssertIndexInBounds(__n, original::size());
		}
		return original::operator[](__n);
	}

	typename original::reference operator[](typename original::size_type __n) { // NOLINT: hiding on purpose
		return get<SAFE>(__n);
	}
	typename original::const_reference operator[](typename original::size_type __n) const { // NOLINT: hiding on purpose
		return get<SAFE>(__n);
	}

	typename original::reference front() { // NOLINT: hiding on purpose
		return get<SAFE>(0);
	}

	typename original::const_reference front() const { // NOLINT: hiding on purpose
		return get<SAFE>(0);
	}

	typename original::reference back() { // NOLINT: hiding on purpose
		if (MemorySafety<SAFE>::ENABLED && original::empty()) {
			throw InternalException("'back' called on an empty vector!");
		}
		return get<SAFE>(original::size() - 1);
	}

	typename original::const_reference back() const { // NOLINT: hiding on purpose
		if (MemorySafety<SAFE>::ENABLED && original::empty()) {
			throw InternalException("'back' called on an empty vector!");
		}
		return get<SAFE>(original::size() - 1);
	}

	void unsafe_erase_at(idx_t idx) { // NOLINT: not using camelcase on purpose here
		original::erase(original::begin() + static_cast<typename original::iterator::difference_type>(idx));
	}

	void erase_at(idx_t idx) { // NOLINT: not using camelcase on purpose here
		if (MemorySafety<SAFE>::ENABLED && idx > original::size()) {
			throw InternalException("Can't remove offset %d from vector of size %d", idx, original::size());
		}
		unsafe_erase_at(idx);
	}
};

template <typename T>
using unsafe_vector = vector<T, false>;

} // namespace goose
