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
#include <goose/planner/logical_operator.h>

namespace goose {

class LogicalPlanStatement : public SQLStatement {
public:
	static constexpr const StatementType TYPE = StatementType::LOGICAL_PLAN_STATEMENT;

public:
	explicit LogicalPlanStatement(unique_ptr<LogicalOperator> plan_p)
	    : SQLStatement(StatementType::LOGICAL_PLAN_STATEMENT), plan(std::move(plan_p)) {};

	unique_ptr<LogicalOperator> plan;

public:
	unique_ptr<SQLStatement> Copy() const override {
		throw NotImplementedException("PLAN_STATEMENT");
	}
	string ToString() const override {
		throw NotImplementedException("PLAN STATEMENT");
	}
};

} // namespace goose
