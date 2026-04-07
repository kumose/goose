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

#include <goose/common/case_insensitive_map.h>
#include <goose/common/enums/index_constraint_type.h>
#include <goose/common/types/value.h>
#include <goose/common/vector.h>
#include <goose/parser/parsed_data/create_info.h>
#include <goose/parser/parsed_expression.h>

namespace goose {

struct CreateIndexInfo : public CreateInfo {
	CreateIndexInfo();
	CreateIndexInfo(const CreateIndexInfo &info);

	//! The table name of the underlying table
	string table;
	//! The name of the index
	string index_name;

	//! Options values (WITH ...)
	case_insensitive_map_t<Value> options;

	//! The index type (ART, B+-tree, Skip-List, ...)
	string index_type;
	//! The index constraint type
	IndexConstraintType constraint_type;
	//! The column ids of the indexed table
	vector<column_t> column_ids;
	//! The set of expressions to index by
	vector<unique_ptr<ParsedExpression>> expressions;
	vector<unique_ptr<ParsedExpression>> parsed_expressions;

	//! The types of the logical columns (necessary for scanning the table during CREATE INDEX)
	vector<LogicalType> scan_types;
	//! The names of the logical columns (necessary for scanning the table during CREATE INDEX)
	vector<string> names;

public:
	GOOSE_API unique_ptr<CreateInfo> Copy() const override;
	string ToString() const override;
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<CreateInfo> Deserialize(Deserializer &deserializer);

	vector<string> ExpressionsToList() const;
	string ExpressionsToString() const;
};

} // namespace goose
