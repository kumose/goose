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

#include <goose/common/constants.h>
#include <goose/common/types-import.h>

namespace goose {

struct SegmentLock {
public:
	SegmentLock() {
	}
	explicit SegmentLock(mutex &lock) : lock(lock) {
	}
	// disable copy constructors
	SegmentLock(const SegmentLock &other) = delete;
	SegmentLock &operator=(const SegmentLock &) = delete;
	//! enable move constructors
	SegmentLock(SegmentLock &&other) noexcept {
		std::swap(lock, other.lock);
	}
	SegmentLock &operator=(SegmentLock &&other) noexcept {
		std::swap(lock, other.lock);
		return *this;
	}

	void Release() {
		lock.unlock();
	}

private:
	unique_lock<mutex> lock;
};

} // namespace goose
