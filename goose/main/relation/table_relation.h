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
