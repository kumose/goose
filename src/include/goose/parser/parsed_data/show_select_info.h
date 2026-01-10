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

#include <goose/parser/parsed_data/parse_info.h>
#include <goose/parser/query_node.h>

namespace goose {

struct ShowSelectInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::SHOW_SELECT_INFO;

public:
	ShowSelectInfo() : ParseInfo(TYPE) {
	}

	//! Types of projected columns
	vector<LogicalType> types;
	//! The QueryNode of select query
	unique_ptr<QueryNode> query;
	//! Aliases of projected columns
	vector<string> aliases;
	//! Whether or not we are requesting a summary or a describe
	bool is_summary;

	unique_ptr<ShowSelectInfo> Copy() {
		auto result = make_uniq<ShowSelectInfo>();
		result->types = types;
		result->query = query->Copy();
		result->aliases = aliases;
		result->is_summary = is_summary;
		return result;
	}
};

} // namespace goose
