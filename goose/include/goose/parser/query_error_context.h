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
