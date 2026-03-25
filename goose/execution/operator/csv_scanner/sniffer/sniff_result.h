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

#include <goose/execution/operator/csv_scanner/csv_state_machine.h>
#include <goose/common/vector.h>

namespace goose {

//! Struct to store the result of the Sniffer
struct SnifferResult {
	SnifferResult(vector<LogicalType> return_types_p, vector<string> names_p)
	    : return_types(std::move(return_types_p)), names(std::move(names_p)) {
	}
	//! Return Types that were detected
	vector<LogicalType> return_types;
	//! Column Names that were detected
	vector<string> names;
};

struct AdaptiveSnifferResult : SnifferResult {
	AdaptiveSnifferResult(vector<LogicalType> return_types_p, vector<string> names_p, bool more_than_one_row_p)
	    : SnifferResult(std::move(return_types_p), std::move(names_p)), more_than_one_row(more_than_one_row_p) {
	}
	bool more_than_one_row;
	SnifferResult ToSnifferResult() {
		return {return_types, names};
	}
};
} // namespace goose
