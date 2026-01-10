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
