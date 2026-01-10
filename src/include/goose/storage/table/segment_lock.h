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
