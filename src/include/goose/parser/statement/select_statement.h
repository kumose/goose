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

#include <goose/parser/query_node.h>
#include <goose/parser/sql_statement.h>
#include <goose/parser/tableref.h>

namespace goose {

class QueryNode;
class Serializer;
class Deserializer;

//! SelectStatement is a typical SELECT clause
class SelectStatement : public SQLStatement {
public:
	static constexpr const StatementType TYPE = StatementType::SELECT_STATEMENT;

public:
	SelectStatement() : SQLStatement(StatementType::SELECT_STATEMENT) {
	}

	//! The main query node
	unique_ptr<QueryNode> node;

protected:
	SelectStatement(const SelectStatement &other);

public:
	//! Convert the SELECT statement to a string

	GOOSE_API string ToString() const override;
	//! Create a copy of this SelectStatement
	GOOSE_API unique_ptr<SQLStatement> Copy() const override;
	//! Whether or not the statements are equivalent
	bool Equals(const SQLStatement &other) const;

	void Serialize(Serializer &serializer) const;
	static unique_ptr<SelectStatement> Deserialize(Deserializer &deserializer);
};
} // namespace goose
