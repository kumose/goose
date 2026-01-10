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

#include <goose/goose.h>
#include <goose/common/allocator.h>

#include <exception>

namespace goose {

class ByteBuffer { // on to the 10 thousandth impl
public:
	ByteBuffer() {};
	ByteBuffer(data_ptr_t ptr, uint64_t len) : ptr(ptr), len(len) {};

	data_ptr_t ptr = nullptr;
	uint64_t len = 0;

public:
	void inc(const uint64_t increment) {
		available(increment);
		unsafe_inc(increment);
	}

	void unsafe_inc(const uint64_t increment) {
		len -= increment;
		ptr += increment;
	}

	template <class T>
	T read() {
		available(sizeof(T));
		return unsafe_read<T>();
	}

	template <class T>
	T unsafe_read() {
		T val = unsafe_get<T>();
		unsafe_inc(sizeof(T));
		return val;
	}

	template <class T>
	T get() {
		available(sizeof(T));
		return unsafe_get<T>();
	}

	template <class T>
	T unsafe_get() {
		return Load<T>(ptr);
	}

	void copy_to(char *dest, const uint64_t len) const {
		available(len);
		unsafe_copy_to(dest, len);
	}

	void unsafe_copy_to(char *dest, const uint64_t len) const {
		std::memcpy(dest, ptr, len);
	}

	void zero() const {
		std::memset(ptr, 0, len);
	}

	void available(const uint64_t req_len) const {
		if (!check_available(req_len)) {
			throw std::runtime_error("Out of buffer");
		}
	}

	bool check_available(const uint64_t req_len) const {
		return req_len <= len;
	}
};

class ResizeableBuffer : public ByteBuffer {
public:
	ResizeableBuffer() {
	}
	ResizeableBuffer(Allocator &allocator, const uint64_t new_size) {
		resize(allocator, new_size);
	}
	void resize(Allocator &allocator, const uint64_t new_size) {
		len = new_size;
		if (new_size == 0) {
			return;
		}
		if (new_size > alloc_len) {
			alloc_len = NextPowerOfTwo(new_size);
			allocated_data.Reset(); // Have to reset before allocating new buffer (otherwise we use ~2x the memory)
			allocated_data = allocator.Allocate(alloc_len);
			ptr = allocated_data.get();
		}
	}
	void reset() {
		ptr = allocated_data.get();
		len = alloc_len;
	}

private:
	AllocatedData allocated_data;
	idx_t alloc_len = 0;
};

} // namespace goose
