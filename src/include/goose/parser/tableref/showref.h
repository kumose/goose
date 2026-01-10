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

#include <goose/parser/tableref.h>
#include <goose/parser/parsed_expression.h>
#include <goose/parser/query_node.h>

namespace goose {

enum class ShowType : uint8_t { SUMMARY, DESCRIBE, SHOW_FROM };

//! Represents a SHOW/DESCRIBE/SUMMARIZE statement
class ShowRef : public TableRef {
public:
	static constexpr const TableReferenceType TYPE = TableReferenceType::SHOW_REF;

public:
	ShowRef();

	//! The table name (if any)
	string table_name;
	//! The catalog name (if any)
	string catalog_name;
	//! The schema name (if any)
	string schema_name;
	//! The QueryNode of select query (if any)
	unique_ptr<QueryNode> query;
	//! Whether or not we are requesting a summary or a describe
	ShowType show_type;

public:
	string ToString() const override;
	bool Equals(const TableRef &other_p) const override;

	unique_ptr<TableRef> Copy() override;

	//! Deserializes a blob back into a ExpressionListRef
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<TableRef> Deserialize(Deserializer &source);
};

} // namespace goose
