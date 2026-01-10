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
