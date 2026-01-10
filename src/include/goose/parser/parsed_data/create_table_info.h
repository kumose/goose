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

#include <goose/parser/parsed_data/create_info.h>
#include <goose/parser/constraint.h>
#include <goose/parser/statement/select_statement.h>
#include <goose/parser/column_list.h>

namespace goose {
class SchemaCatalogEntry;

struct CreateTableInfo : public CreateInfo {
	GOOSE_API CreateTableInfo();
	GOOSE_API CreateTableInfo(string catalog, string schema, string name);
	GOOSE_API CreateTableInfo(SchemaCatalogEntry &schema, string name);

	//! Table name to insert to
	string table;
	//! List of columns of the table
	ColumnList columns;
	//! List of constraints on the table
	vector<unique_ptr<Constraint>> constraints;
	//! CREATE TABLE as QUERY
	unique_ptr<SelectStatement> query;

public:
	GOOSE_API unique_ptr<CreateInfo> Copy() const override;

	GOOSE_API void Serialize(Serializer &serializer) const override;
	GOOSE_API static unique_ptr<CreateInfo> Deserialize(Deserializer &deserializer);

	string ToString() const override;
};

} // namespace goose
