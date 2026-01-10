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
#include <goose/parser/group_by_node.h>

namespace goose {

class AggregateRelation : public Relation {
public:
	GOOSE_API AggregateRelation(shared_ptr<Relation> child, vector<unique_ptr<ParsedExpression>> expressions);
	GOOSE_API AggregateRelation(shared_ptr<Relation> child, vector<unique_ptr<ParsedExpression>> expressions,
	                             GroupByNode groups);
	GOOSE_API AggregateRelation(shared_ptr<Relation> child, vector<unique_ptr<ParsedExpression>> expressions,
	                             vector<unique_ptr<ParsedExpression>> groups);

	vector<unique_ptr<ParsedExpression>> expressions;
	GroupByNode groups;
	vector<ColumnDefinition> columns;
	shared_ptr<Relation> child;

public:
	unique_ptr<QueryNode> GetQueryNode() override;

	const vector<ColumnDefinition> &Columns() override;
	string ToString(idx_t depth) override;
	string GetAlias() override;
};

} // namespace goose
