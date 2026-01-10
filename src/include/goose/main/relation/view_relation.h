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

#include <goose/main/relation.h>

namespace goose {

class ViewRelation : public Relation {
public:
	ViewRelation(const shared_ptr<ClientContext> &context, string schema_name, string view_name);
	ViewRelation(const shared_ptr<RelationContextWrapper> &context, string schema_name, string view_name);
	ViewRelation(const shared_ptr<ClientContext> &context, unique_ptr<TableRef> ref, const string &view_name);

	string schema_name;
	string view_name;
	vector<ColumnDefinition> columns;
	unique_ptr<TableRef> premade_tableref;

public:
	unique_ptr<QueryNode> GetQueryNode() override;
	unique_ptr<TableRef> GetTableRef() override;

	const vector<ColumnDefinition> &Columns() override;
	string ToString(idx_t depth) override;
	string GetAlias() override;
};

} // namespace goose
