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
#include <goose/execution/operator/csv_scanner/sniffer/sniff_result.h>

namespace goose {
//! Basic CSV Column Info
struct CSVColumnInfo {
	CSVColumnInfo(const string &name_p, const LogicalType &type_p) : name(name_p), type(type_p) {
	}
	string name;
	LogicalType type;
};

//! Basic CSV Schema
struct CSVSchema {
	explicit CSVSchema(const bool empty = false) : empty(empty) {
	}

	CSVSchema(const vector<string> &names, const vector<LogicalType> &types, const string &file_path, idx_t rows_read,
	          const bool empty = false);

	//! Initializes the schema based on names and types
	void Initialize(const vector<string> &names, const vector<LogicalType> &types, const string &file_path);

	//! If the schema is empty
	bool Empty() const;

	//! If the columns of the schema match
	bool MatchColumns(const CSVSchema &other) const;

	//! We merge two schemas by ensuring that the column types are compatible between both
	void MergeSchemas(CSVSchema &other, bool null_padding);

	//! What's the file path for the file that generated this schema
	string GetPath() const;

	//! How many columns we have in this schema
	idx_t GetColumnCount() const;

	//! Check if two schemas match.
	bool SchemasMatch(string &error_message, SnifferResult &sniffer_result, const string &cur_file_path,
	                  bool is_minimal_sniffer) const;

	//! How many rows were read when generating this schema, this is only used for sniffing during the binder
	idx_t GetRowsRead() const;

	//! Get a vector with names
	vector<string> GetNames() const;

	//! Get a vector with types
	vector<LogicalType> GetTypes() const;

	//! Replace any SQLNull types with Varchar
	void ReplaceNullWithVarchar();

private:
	//! If a type can be cast to another
	static bool CanWeCastIt(LogicalTypeId source, LogicalTypeId destination);
	vector<CSVColumnInfo> columns;
	unordered_map<string, idx_t> name_idx_map;
	string file_path;
	idx_t rows_read = 0;
	bool empty = false;
};
} // namespace goose
