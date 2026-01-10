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

struct ArrowSchema;

namespace goose {

struct ArrowBuffer {
	static constexpr const idx_t MINIMUM_SHRINK_SIZE = 4096;

	ArrowBuffer() : dataptr(nullptr), count(0), capacity(0) {
	}
	~ArrowBuffer() {
		if (!dataptr) {
			return;
		}
		free(dataptr);
		dataptr = nullptr;
		count = 0;
		capacity = 0;
	}
	// disable copy constructors
	ArrowBuffer(const ArrowBuffer &other) = delete;
	ArrowBuffer &operator=(const ArrowBuffer &) = delete;
	//! enable move constructors
	ArrowBuffer(ArrowBuffer &&other) noexcept : count(0), capacity(0) {
		std::swap(dataptr, other.dataptr);
		std::swap(count, other.count);
		std::swap(capacity, other.capacity);
	}
	ArrowBuffer &operator=(ArrowBuffer &&other) noexcept {
		std::swap(dataptr, other.dataptr);
		std::swap(count, other.count);
		std::swap(capacity, other.capacity);
		return *this;
	}

	void reserve(idx_t bytes) { // NOLINT
		auto new_capacity = NextPowerOfTwo(bytes);
		if (new_capacity <= capacity) {
			return;
		}
		ReserveInternal(new_capacity);
	}

	void resize(idx_t bytes) { // NOLINT
		reserve(bytes);
		count = bytes;
	}

	void resize(idx_t bytes, data_t value) { // NOLINT
		reserve(bytes);
		for (idx_t i = count; i < bytes; i++) {
			dataptr[i] = value;
		}
		count = bytes;
	}

	template <class T>
	void push_back(T value) {
		reserve(sizeof(T) * (count + 1));
		reinterpret_cast<T *>(dataptr)[count] = value;
		count++;
	}

	idx_t size() { // NOLINT
		return count;
	}

	data_ptr_t data() { // NOLINT
		return dataptr;
	}

	template <class T>
	T *GetData() {
		return reinterpret_cast<T *>(data());
	}

private:
	void ReserveInternal(idx_t bytes) {
		if (dataptr) {
			dataptr = data_ptr_cast(realloc(dataptr, bytes));
		} else {
			dataptr = data_ptr_cast(malloc(bytes));
		}
		capacity = bytes;
	}

private:
	data_ptr_t dataptr = nullptr;
	idx_t count = 0;
	idx_t capacity = 0;
};

} // namespace goose
