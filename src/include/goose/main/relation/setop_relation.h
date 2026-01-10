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
#include <goose/common/enums/set_operation_type.h>

namespace goose {

class SetOpRelation : public Relation {
public:
	SetOpRelation(shared_ptr<Relation> left, shared_ptr<Relation> right, SetOperationType setop_type,
	              bool setop_all = false);

	shared_ptr<Relation> left;
	shared_ptr<Relation> right;
	SetOperationType setop_type;
	vector<ColumnDefinition> columns;
	bool setop_all;

public:
	unique_ptr<QueryNode> GetQueryNode() override;

	const vector<ColumnDefinition> &Columns() override;
	string ToString(idx_t depth) override;
	string GetAlias() override;
};

} // namespace goose
