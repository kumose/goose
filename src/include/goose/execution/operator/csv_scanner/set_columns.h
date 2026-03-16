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

#include <goose/common/types.h>

namespace goose {
//! This represents the data related to columns that have been set by the user
//! e.g., from a copy command
struct SetColumns {
	SetColumns(const vector<LogicalType> *types_p, const vector<string> *names_p);
	SetColumns();
	//! Return Types that were detected
	const vector<LogicalType> *types = nullptr;
	//! Column Names that were detected
	const vector<string> *names = nullptr;
	//! If columns are set
	bool IsSet() const;
	//! How many columns
	idx_t Size() const;
	//! Helper function that checks if candidate is acceptable based on the number of columns it produces
	bool IsCandidateUnacceptable(const idx_t num_cols, bool null_padding, bool ignore_errors,
	                             bool last_value_always_empty) const;

	string ToString() const;
};

} // namespace goose
