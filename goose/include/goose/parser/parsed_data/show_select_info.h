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
