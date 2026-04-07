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

#include <goose/parallel/async_result.h>

#include <chrono>
#include <thread>

namespace goose {

class SleepAsyncTask : public AsyncTask {
public:
	explicit SleepAsyncTask(idx_t sleep_for) : sleep_for(sleep_for) {
	}
	void Execute() override {
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_for));
	}
	const idx_t sleep_for;
};

class ThrowAsyncTask : public AsyncTask {
public:
	explicit ThrowAsyncTask(idx_t sleep_for) : sleep_for(sleep_for) {
	}
	void Execute() override {
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_for));
		throw NotImplementedException("ThrowAsyncTask: Test error handling when throwing mid-task");
	}
	const idx_t sleep_for;
};

} // namespace goose
