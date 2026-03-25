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
