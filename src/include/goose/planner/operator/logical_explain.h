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

#include <goose/parser/statement/explain_statement.h>
#include <goose/planner/logical_operator.h>

namespace goose {

class LogicalExplain : public LogicalOperator {
	explicit LogicalExplain(ExplainType explain_type)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_EXPLAIN), explain_type(explain_type) {};

public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_EXPLAIN;

public:
	LogicalExplain(unique_ptr<LogicalOperator> plan, ExplainType explain_type, ExplainFormat explain_format)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_EXPLAIN), explain_type(explain_type),
	      explain_format(explain_format) {
		children.push_back(std::move(plan));
	}

	ExplainType explain_type;
	ExplainFormat explain_format;
	string physical_plan;
	string logical_plan_unopt;
	string logical_plan_opt;

public:
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	idx_t EstimateCardinality(ClientContext &context) override {
		return 3;
	}
	//! Skips the serialization check in VerifyPlan
	bool SupportSerialization() const override {
		return false;
	}

protected:
	void ResolveTypes() override {
		types = {LogicalType::VARCHAR, LogicalType::VARCHAR};
	}
	vector<ColumnBinding> GetColumnBindings() override {
		return {ColumnBinding(0, 0), ColumnBinding(0, 1)};
	}
};
} // namespace goose
