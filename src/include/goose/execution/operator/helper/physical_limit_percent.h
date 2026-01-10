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

#include <goose/execution/physical_operator.h>
#include <goose/planner/expression.h>
#include <goose/planner/bound_result_modifier.h>

namespace goose {

//! PhyisicalLimitPercent represents the LIMIT PERCENT operator
class PhysicalLimitPercent : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::LIMIT_PERCENT;

public:
	PhysicalLimitPercent(PhysicalPlan &physical_plan, vector<LogicalType> types, BoundLimitNode limit_val_p,
	                     BoundLimitNode offset_val_p, idx_t estimated_cardinality);

	BoundLimitNode limit_val;
	BoundLimitNode offset_val;

public:
	bool SinkOrderDependent() const override {
		return true;
	}

public:
	// Source interface
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

public:
	// Sink Interface
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;

	bool IsSink() const override {
		return true;
	}
};

} // namespace goose
