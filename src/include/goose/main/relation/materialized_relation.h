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

class MaterializedRelation : public Relation {
public:
	MaterializedRelation(const shared_ptr<ClientContext> &context, unique_ptr<ColumnDataCollection> &&collection,
	                     vector<string> names, string alias = "materialized");
	vector<ColumnDefinition> columns;
	string alias;
	shared_ptr<ColumnDataCollection> collection;

public:
	const vector<ColumnDefinition> &Columns() override;
	string ToString(idx_t depth) override;
	string GetAlias() override;
	unique_ptr<TableRef> GetTableRef() override;
	unique_ptr<QueryNode> GetQueryNode() override;
};

} // namespace goose
