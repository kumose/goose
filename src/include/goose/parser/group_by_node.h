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

#include <goose/parser/parsed_expression.h>
#include <goose/common/types-import.h>
#include <goose/common/vector.h>

namespace goose {

using GroupingSet = set<idx_t>;

class GroupByNode {
public:
	//! The total set of all group expressions
	vector<unique_ptr<ParsedExpression>> group_expressions;
	//! The different grouping sets as they map to the group expressions
	vector<GroupingSet> grouping_sets;

public:
	GroupByNode Copy() {
		GroupByNode node;
		node.group_expressions.reserve(group_expressions.size());
		for (auto &expr : group_expressions) {
			node.group_expressions.push_back(expr->Copy());
		}
		node.grouping_sets = grouping_sets;
		return node;
	}
};

} // namespace goose
