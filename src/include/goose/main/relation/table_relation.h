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
#include <goose/main/table_description.h>

namespace goose {

class TableRelation : public Relation {
public:
	TableRelation(const shared_ptr<ClientContext> &context, unique_ptr<TableDescription> description);
	TableRelation(const shared_ptr<RelationContextWrapper> &context, unique_ptr<TableDescription> description);

	unique_ptr<TableDescription> description;

public:
	unique_ptr<QueryNode> GetQueryNode() override;

	const vector<ColumnDefinition> &Columns() override;
	string ToString(idx_t depth) override;
	string GetAlias() override;

	unique_ptr<TableRef> GetTableRef() override;

	void Insert(const vector<vector<Value>> &values) override;
	void Insert(vector<vector<unique_ptr<ParsedExpression>>> &&expressions) override;
	void Update(const string &update, const string &condition = string()) override;
	void Update(vector<string> column_names, vector<unique_ptr<ParsedExpression>> &&update,
	            unique_ptr<ParsedExpression> condition = nullptr) override;
	void Delete(const string &condition = string()) override;
};

} // namespace goose
