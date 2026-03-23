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

#ifndef GOOSE_NO_THREADS
#include <goose/common/concurrentqueue/concurrentqueue.h>
#else

#include <cstddef>
#include <deque>
#include <queue>

namespace goose_moodycamel {

template <typename T>
class ConcurrentQueue;
template <typename T>
class BlockingConcurrentQueue;

struct ProducerToken {
	//! Constructor
	template <typename T, typename Traits>
	explicit ProducerToken(ConcurrentQueue<T> &);
	//! Constructor
	template <typename T, typename Traits>
	explicit ProducerToken(BlockingConcurrentQueue<T> &);
	//! Constructor
	ProducerToken(ProducerToken &&) {
	}
	//! Is valid token?
	inline bool valid() const {
		return true;
	}
};

template <typename T>
class ConcurrentQueue {
private:
	//! The standard library queue.
	std::queue<T, std::deque<T>> q;

public:
	//! Default constructor.
	ConcurrentQueue() = default;
	//! Constructor reserving capacity.
	explicit ConcurrentQueue(size_t capacity) {
		q.reserve(capacity);
	}

	//! Enqueue an item.
	template <typename U>
	bool enqueue(U &&item) {
		q.push(std::forward<U>(item));
		return true;
	}
	//! Try to dequeue an item.
	bool try_dequeue(T &item) {
		if (q.empty()) {
			return false;
		}
		item = std::move(q.front());
		q.pop();
		return true;
	}
	//! Get the size of the queue.
	size_t size_approx() const {
		return q.size();
	}
	//! Dequeues several elements from the queue.
	//! Returns the number of elements dequeued.
	template <typename It>
	size_t try_dequeue_bulk(It itemFirst, size_t max) {
		for (size_t i = 0; i < max; i++) {
			if (!try_dequeue(*itemFirst)) {
				return i;
			}
			itemFirst++;
		}
		return max;
	}

	template <typename It>
	bool enqueue_bulk(It itemFirst, size_t count) {
		for (size_t i = 0; i < count; i++) {
			q.push(std::move(*itemFirst++));
		}
		return true;
	}
};

} // namespace goose_moodycamel

#endif
