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
#include <goose/common/shared_ptr.h>
#include <goose/common/unique_ptr.h>

namespace goose {

template <class T, bool SAFE = true>
class atomic_ptr { // NOLINT: mimic std casing
public:
	atomic_ptr() noexcept : ptr(nullptr) {
	}
	atomic_ptr(T *ptr_p) : ptr(ptr_p) { // NOLINT: allow implicit creation from pointer
	}
	atomic_ptr(T &ref) : ptr(&ref) { // NOLINT: allow implicit creation from reference
	}
	atomic_ptr(const unique_ptr<T> &ptr_p) : ptr(ptr_p.get()) { // NOLINT: allow implicit creation from unique pointer
	}
	atomic_ptr(const shared_ptr<T> &ptr_p) : ptr(ptr_p.get()) { // NOLINT: allow implicit creation from shared pointer
	}

	void CheckValid(const T *ptr) const {
		if (MemorySafety<SAFE>::ENABLED) {
			return;
		}
		if (!ptr) {
			throw InternalException("Attempting to dereference an optional pointer that is not set");
		}
	}

	T *GetPointer() {
		auto res = ptr.load();
		CheckValid(res);
		return res;
	}

	const T *GetPointer() const {
		auto res = ptr.load();
		CheckValid(res);
		return res;
	}

	operator bool() const { // NOLINT: allow implicit conversion to bool
		return ptr;
	}
	T &operator*() {
		return *GetPointer();
	}
	const T &operator*() const {
		return *GetPointer();
	}
	T *operator->() {
		return GetPointer();
	}
	const T *operator->() const {
		return GetPointer();
	}
	T *get() { // NOLINT: mimic std casing
		return GetPointer();
	}
	const T *get() const { // NOLINT: mimic std casing
		return GetPointer();
	}
	// this looks dirty - but this is the default behavior of raw pointers
	T *get_mutable() const { // NOLINT: mimic std casing
		return GetPointer();
	}

	void set(T &ref) {
		ptr = &ref;
	}

	void reset() {
		ptr = nullptr;
	}

	bool operator==(const atomic_ptr<T> &rhs) const {
		return ptr.load() == rhs.ptr.load();
	}

	bool operator!=(const atomic_ptr<T> &rhs) const {
		return ptr.load() != rhs.ptr.load();
	}

private:
	atomic<T *> ptr;
};

template <typename T>
using unsafe_atomic_ptr = atomic_ptr<T, false>;

} // namespace goose
