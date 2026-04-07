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
#include <goose/common/vector.h>
#include <goose/common/types-import.h>
#include <goose/common/types/value.h>
#include <goose/common/case_insensitive_map.h>
#include <goose/parser/query_node.h>

namespace goose {

class QueryNode;

struct CopyInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::COPY_INFO;

public:
	CopyInfo();

	//! The catalog name to copy to/from
	string catalog;
	//! The schema name to copy to/from
	string schema;
	//! The table name to copy to/from
	string table;
	//! List of columns to copy to/from
	vector<string> select_list;
	//! Whether or not this is a copy to file (false) or copy from a file (true)
	bool is_from;
	//! The file format of the external file
	string format;
	//! If the format is manually set (i.e., via the format parameter) or was discovered by inspecting the file path
	bool is_format_auto_detected;
	//! Expression to determine the file path (if any)
	unique_ptr<ParsedExpression> file_path_expression;
	//! The file path to copy to/from
	string file_path;
	//! Set of (key, value) options
	case_insensitive_map_t<unique_ptr<ParsedExpression>> parsed_options;
	//! Set of (key, value) options
	case_insensitive_map_t<vector<Value>> options;
	//! The SQL statement used instead of a table when copying data out to a file
	unique_ptr<QueryNode> select_statement;

public:
	string CopyOptionsToString() const;

public:
	unique_ptr<CopyInfo> Copy() const;
	string ToString() const;
	string TablePartToString() const;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);
};

} // namespace goose
