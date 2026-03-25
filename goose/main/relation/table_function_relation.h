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

namespace goose {

class TableFunctionRelation : public Relation {
public:
	TableFunctionRelation(const shared_ptr<ClientContext> &context, string name, vector<Value> parameters,
	                      named_parameter_map_t named_parameters, shared_ptr<Relation> input_relation_p = nullptr,
	                      bool auto_init = true);
	TableFunctionRelation(const shared_ptr<RelationContextWrapper> &context, string name, vector<Value> parameters,
	                      named_parameter_map_t named_parameters, shared_ptr<Relation> input_relation_p = nullptr,
	                      bool auto_init = true);
	TableFunctionRelation(const shared_ptr<ClientContext> &context, string name, vector<Value> parameters,
	                      shared_ptr<Relation> input_relation_p = nullptr, bool auto_init = true);
	~TableFunctionRelation() override {
	}

	string name;
	vector<Value> parameters;
	named_parameter_map_t named_parameters;
	vector<ColumnDefinition> columns;
	shared_ptr<Relation> input_relation;

public:
	unique_ptr<QueryNode> GetQueryNode() override;
	unique_ptr<TableRef> GetTableRef() override;

	const vector<ColumnDefinition> &Columns() override;
	string ToString(idx_t depth) override;
	string GetAlias() override;
	void AddNamedParameter(const string &name, Value argument);
	void RemoveNamedParameterIfExists(const string &name);
	void SetNamedParameters(named_parameter_map_t &&named_parameters);

private:
	void InitializeColumns();

private:
	//! Whether or not to auto initialize the columns on construction
	bool auto_initialize;
};

} // namespace goose
