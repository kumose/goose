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

#include <goose/common/assert.h>
#include <goose/common/typedefs.h>
#include <goose/common/types-import.h>
#include <goose/common/exception.h>
#include <goose/common/memory_safety.h>
#include <queue>

namespace goose {

template <class DATA_TYPE, class CONTAINER = std::deque<DATA_TYPE>, bool SAFE = true>
class queue : public std::queue<DATA_TYPE, CONTAINER> { // NOLINT: matching name of std
public:
	using original = std::queue<DATA_TYPE, CONTAINER>;
	using original::original;
	using container_type = typename original::container_type;
	using value_type = typename original::value_type;
	using size_type = typename container_type::size_type;
	using reference = typename container_type::reference;
	using const_reference = typename container_type::const_reference;

public:
	// Because we create the other constructor, the implicitly created constructor
	// gets deleted, so we have to be explicit
	queue() = default;
	queue(original &&other) : original(std::move(other)) { // NOLINT: allow implicit conversion
	}
	template <bool INTERNAL_SAFE>
	queue(queue<DATA_TYPE, CONTAINER, INTERNAL_SAFE> &&other) : original(std::move(other)) { // NOLINT
	}

	inline void clear() noexcept {
		original::c.clear();
	}

	reference front() {
		if (MemorySafety<SAFE>::ENABLED && original::empty()) {
			throw InternalException("'front' called on an empty queue!");
		}
		return original::front();
	}

	const_reference front() const {
		if (MemorySafety<SAFE>::ENABLED && original::empty()) {
			throw InternalException("'front' called on an empty queue!");
		}
		return original::front();
	}

	reference back() {
		if (MemorySafety<SAFE>::ENABLED && original::empty()) {
			throw InternalException("'back' called on an empty queue!");
		}
		return original::back();
	}

	const_reference back() const {
		if (MemorySafety<SAFE>::ENABLED && original::empty()) {
			throw InternalException("'back' called on an empty queue!");
		}
		return original::back();
	}

	void pop() {
		if (MemorySafety<SAFE>::ENABLED && original::empty()) {
			throw InternalException("'pop' called on an empty queue!");
		}
		original::pop();
	}
};

template <typename T, typename Container = std::deque<T>>
using unsafe_queue = queue<T, Container, false>;

} // namespace goose
