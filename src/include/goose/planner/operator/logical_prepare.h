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
