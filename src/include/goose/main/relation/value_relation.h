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
#include <goose/parser/parsed_expression.h>

namespace goose {

class ValueRelation : public Relation {
public:
	ValueRelation(const shared_ptr<ClientContext> &context, const vector<vector<Value>> &values, vector<string> names,
	              string alias = "values");
	ValueRelation(const shared_ptr<ClientContext> &context, vector<vector<unique_ptr<ParsedExpression>>> &&expressions,
	              vector<string> names, string alias = "values");
	ValueRelation(const shared_ptr<RelationContextWrapper> &context, const vector<vector<Value>> &values,
	              vector<string> names, string alias = "values");
	ValueRelation(const shared_ptr<RelationContextWrapper> &context,
	              vector<vector<unique_ptr<ParsedExpression>>> &&expressions, vector<string> names,
	              string alias = "values");
	ValueRelation(const shared_ptr<ClientContext> &context, const string &values, vector<string> names,
	              string alias = "values");

	vector<vector<unique_ptr<ParsedExpression>>> expressions;
	vector<string> names;
	vector<ColumnDefinition> columns;
	string alias;

public:
	unique_ptr<QueryNode> GetQueryNode() override;

	const vector<ColumnDefinition> &Columns() override;
	string ToString(idx_t depth) override;
	string GetAlias() override;

	unique_ptr<TableRef> GetTableRef() override;
};

} // namespace goose
