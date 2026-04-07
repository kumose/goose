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
