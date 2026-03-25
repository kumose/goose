// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
