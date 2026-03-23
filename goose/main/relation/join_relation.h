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

#include <goose/main/relation.h>
#include <goose/common/enums/joinref_type.h>

namespace goose {

class JoinRelation : public Relation {
public:
	GOOSE_API JoinRelation(shared_ptr<Relation> left, shared_ptr<Relation> right,
	                        unique_ptr<ParsedExpression> condition, JoinType type,
	                        JoinRefType join_ref_type = JoinRefType::REGULAR);
	GOOSE_API JoinRelation(shared_ptr<Relation> left, shared_ptr<Relation> right, vector<string> using_columns,
	                        JoinType type, JoinRefType join_ref_type = JoinRefType::REGULAR);

	shared_ptr<Relation> left;
	shared_ptr<Relation> right;
	unique_ptr<ParsedExpression> condition;
	vector<string> using_columns;
	JoinType join_type;
	JoinRefType join_ref_type;
	vector<ColumnDefinition> columns;

	vector<unique_ptr<ParsedExpression>> duplicate_eliminated_columns;
	bool delim_flipped = false;

public:
	unique_ptr<QueryNode> GetQueryNode() override;

	const vector<ColumnDefinition> &Columns() override;
	string ToString(idx_t depth) override;

	unique_ptr<TableRef> GetTableRef() override;
};

} // namespace goose
