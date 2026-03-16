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

#include <goose/parser/sql_statement.h>
#include <goose/planner/binder.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/bound_parameter_map.h>

namespace goose {
class ClientContext;
class PreparedStatementData;

//! The planner creates a logical query plan from the parsed SQL statements
//! using the Binder and LogicalPlanGenerator.
class Planner {
	friend class Binder;

public:
	explicit Planner(ClientContext &context);

public:
	unique_ptr<LogicalOperator> plan;
	vector<string> names;
	vector<LogicalType> types;
	case_insensitive_map_t<BoundParameterData> parameter_data;

	shared_ptr<Binder> binder;
	ClientContext &context;

	StatementProperties properties;
	bound_parameter_map_t value_map;

public:
	void CreatePlan(unique_ptr<SQLStatement> statement);
	static void VerifyPlan(ClientContext &context, unique_ptr<LogicalOperator> &op,
	                       optional_ptr<bound_parameter_map_t> map = nullptr);

private:
	void CreatePlan(SQLStatement &statement);
	shared_ptr<PreparedStatementData> PrepareSQLStatement(unique_ptr<SQLStatement> statement);
};
} // namespace goose
