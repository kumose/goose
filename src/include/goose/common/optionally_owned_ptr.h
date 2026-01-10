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

#include <goose/common/exception.h>
#include <goose/common/optional_ptr.h>
#include <goose/common/unique_ptr.h>

namespace goose {

template <class T>
class optionally_owned_ptr { // NOLINT: mimic std casing
public:
	optionally_owned_ptr() {
	}
	optionally_owned_ptr(T *ptr_p) : ptr(ptr_p) { // NOLINT: allow implicit creation from pointer
	}
	optionally_owned_ptr(T &ref) : ptr(&ref) { // NOLINT: allow implicit creation from reference
	}
	optionally_owned_ptr(unique_ptr<T> &&owned_p) // NOLINT: allow implicit creation from moved unique_ptr
	    : owned(std::move(owned_p)), ptr(owned) {
	}
	optionally_owned_ptr(shared_ptr<T> owned_p) // NOLINT: allow implicit creation from shared_ptr
	    : owned_shared(std::move(owned_p)), ptr(*owned_shared) {
	}
	// Move constructor
	optionally_owned_ptr(optionally_owned_ptr &&other) noexcept
	    : owned(std::move(other.owned)), owned_shared(std::move(other.owned_shared)), ptr(other.ptr) {
		other.ptr = nullptr;
	}
	// Copy constructor
	optionally_owned_ptr(const optionally_owned_ptr &other) = delete;

	operator bool() const { // NOLINT: allow implicit conversion to bool
		return ptr;
	}
	T &operator*() {
		return *ptr;
	}
	const T &operator*() const {
		return *ptr;
	}
	T *operator->() {
		return ptr.get();
	}
	const T *operator->() const {
		return ptr.get();
	}
	T *get() { // NOLINT: mimic std casing
		return ptr.get();
	}
	const T *get() const { // NOLINT: mimic std casing
		return ptr.get();
	}
	bool is_owned() const { // NOLINT: mimic std casing
		return owned != nullptr || owned_shared != nullptr;
	}
	const unique_ptr<T> &get_owned_unique() const {
		return owned;
	}
	const shared_ptr<T> &get_owned_shared() const {
		return owned_shared;
	}
	// this looks dirty - but this is the default behavior of raw pointers
	T *get_mutable() const { // NOLINT: mimic std casing
		return ptr.get();
	}

	optionally_owned_ptr<T> &operator=(T &ref) {
		owned = nullptr;
		owned_shared = nullptr;
		ptr = optional_ptr<T>(ref);
		return *this;
	}
	optionally_owned_ptr<T> &operator=(T *ref) {
		owned = nullptr;
		owned_shared = nullptr;
		ptr = optional_ptr<T>(ref);
		return *this;
	}

	bool operator==(const optionally_owned_ptr<T> &rhs) const {
		if (owned != rhs.owned) {
			return false;
		}
		if (owned_shared != rhs.owned_shared) {
			return false;
		}
		return ptr == rhs.ptr;
	}

	bool operator!=(const optionally_owned_ptr<T> &rhs) const {
		return !(*this == rhs);
	}

private:
	unique_ptr<T> owned;
	shared_ptr<T> owned_shared;
	optional_ptr<T> ptr;
};

} // namespace goose
