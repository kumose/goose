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

#include <goose/common/optional_idx.h>

namespace goose {
class ParsedExpression;

class QueryErrorContext {
public:
	QueryErrorContext(const ParsedExpression &expr); // NOLINT: allow implicit conversion from expression
	explicit QueryErrorContext(optional_idx query_location_p = optional_idx()) : query_location(query_location_p) {
	}

	//! The location in which the error should be thrown
	optional_idx query_location;

public:
	static string Format(const string &query, const string &error_message, optional_idx error_loc,
	                     bool add_line_indicator = true);
};

} // namespace goose
