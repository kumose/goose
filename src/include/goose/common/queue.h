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
