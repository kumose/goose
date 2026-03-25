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

#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/main/prepared_statement_data.h>
#include <goose/planner/logical_operator.h>

namespace goose {

class TableCatalogEntry;

class LogicalPrepare : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_PREPARE;

public:
	LogicalPrepare(string name_p, shared_ptr<PreparedStatementData> prepared, unique_ptr<LogicalOperator> logical_plan)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_PREPARE), name(std::move(name_p)),
	      prepared(std::move(prepared)) {
		if (logical_plan) {
			children.push_back(std::move(logical_plan));
		}
	}

	string name;
	shared_ptr<PreparedStatementData> prepared;

public:
	idx_t EstimateCardinality(ClientContext &context) override;
	//! Skips the serialization check in VerifyPlan
	bool SupportSerialization() const override {
		return false;
	}

protected:
	void ResolveTypes() override {
		types.emplace_back(LogicalType::BOOLEAN);
	}

	bool RequireOptimizer() const override {
		if (!prepared->properties.bound_all_parameters || prepared->properties.always_require_rebind) {
			return false;
		}
		return children[0]->RequireOptimizer();
	}
};
} // namespace goose
