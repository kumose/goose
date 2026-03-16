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

#include <goose/common/common.h>
#include <goose/common/shared_ptr.h>
#include <goose/common/types/value.h>

namespace goose {

struct ExtendedOpenFileInfo {
	unordered_map<string, Value> options;
};

struct OpenFileInfo {
	OpenFileInfo() = default;
	OpenFileInfo(string path_p) // NOLINT: allow implicit conversion from string
	    : path(std::move(path_p)) {
	}

	string path;
	shared_ptr<ExtendedOpenFileInfo> extended_info;

public:
	bool operator<(const OpenFileInfo &rhs) const {
		return path < rhs.path;
	}
};

} // namespace goose
