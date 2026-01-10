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
