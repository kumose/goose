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

#include <goose/common/error_data.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/vector.h>

namespace goose {

class TaskErrorManager {
public:
	TaskErrorManager() : has_error(false) {
	}

	void PushError(ErrorData error) {
		lock_guard<mutex> elock(error_lock);
		this->exceptions.push_back(std::move(error));
		has_error = true;
	}

	ErrorData GetError() {
		lock_guard<mutex> elock(error_lock);
		D_ASSERT(!exceptions.empty());

		// FIXME: Should we try to get the biggest priority error?
		// In case the first exception is a StandardException but a regular Exception or a FatalException occurred
		// Maybe we should throw the more critical exception instead, as that changes behavior.
		auto &entry = exceptions[0];
		return entry;
	}

	bool HasError() {
		return has_error;
	}

	void ThrowException() {
		lock_guard<mutex> elock(error_lock);
		D_ASSERT(!exceptions.empty());
		auto &entry = exceptions[0];
		entry.Throw();
	}

	void Reset() {
		lock_guard<mutex> elock(error_lock);
		exceptions.clear();
		has_error = false;
	}

private:
	mutex error_lock;
	//! Exceptions that occurred during the execution of the current query
	vector<ErrorData> exceptions;
	//! Lock-free error flag
	atomic<bool> has_error;
};
} // namespace goose
