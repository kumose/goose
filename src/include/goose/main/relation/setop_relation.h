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
