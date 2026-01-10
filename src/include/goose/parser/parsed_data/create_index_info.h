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
