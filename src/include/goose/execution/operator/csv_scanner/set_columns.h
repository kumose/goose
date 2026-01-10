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
