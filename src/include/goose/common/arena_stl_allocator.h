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

#include <goose/storage/arena_allocator.h>

namespace goose {

template <class T>
class arena_stl_allocator { // NOLINT: match stl case
public:
	//! Typedefs
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef value_type &reference;
	typedef value_type const &const_reference;
	typedef value_type *pointer;
	typedef value_type const *const_pointer;

	//! Propagation traits
	using propagate_on_container_copy_assignment = std::true_type;
	using propagate_on_container_move_assignment = std::true_type;
	using propagate_on_container_swap = std::true_type;
	using is_always_equal = std::false_type;

	//! Rebind
	template <class U>
	struct rebind {
		using other = arena_stl_allocator<U>;
	};

public:
	arena_stl_allocator(ArenaAllocator &arena_allocator_p) noexcept // NOLINT: allow implicit conversion
	    : arena_allocator(arena_allocator_p) {
	}
	template <class U>
	arena_stl_allocator(const arena_stl_allocator<U> &other) noexcept // NOLINT: allow implicit conversion
	    : arena_allocator(other.GetAllocator()) {
	}

public:
	pointer allocate(size_type n) { // NOLINT: match stl case
		arena_allocator.get().AlignNext();
		return reinterpret_cast<pointer>(arena_allocator.get().Allocate(n * sizeof(T)));
	}

	void deallocate(pointer p, size_type n) noexcept { // NOLINT: match stl case
	}

	template <class U, class... Args>
	void construct(U *p, Args &&...args) { // NOLINT: match stl case
		::new (p) U(std::forward<Args>(args)...);
	}

	template <class U>
	void destroy(U *p) noexcept { // NOLINT: match stl case
		p->~U();
	}

	pointer address(reference x) const { // NOLINT: match stl case
		return &x;
	}

	const_pointer address(const_reference x) const { // NOLINT: match stl case
		return &x;
	}

	ArenaAllocator &GetAllocator() const {
		return arena_allocator.get();
	}

public:
	bool operator==(const arena_stl_allocator &other) const noexcept {
		return RefersToSameObject(arena_allocator, other.arena_allocator);
	}
	bool operator!=(const arena_stl_allocator &other) const noexcept {
		return !(*this == other);
	}

private:
	//! Need to use std::reference_wrapper because "reference" is already a typedef
	std::reference_wrapper<ArenaAllocator> arena_allocator;
};

} // namespace goose
