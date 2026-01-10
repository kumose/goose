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

namespace goose {

//! PhysicalStreamingWindow implements streaming window functions (i.e. with an empty OVER clause)
class PhysicalStreamingWindow : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::STREAMING_WINDOW;

	static bool IsStreamingFunction(ClientContext &context, unique_ptr<Expression> &expr);

public:
	PhysicalStreamingWindow(PhysicalPlan &physical_plan, vector<LogicalType> types,
	                        vector<unique_ptr<Expression>> select_list, idx_t estimated_cardinality,
	                        PhysicalOperatorType type = PhysicalOperatorType::STREAMING_WINDOW);

	//! The projection list of the WINDOW statement
	vector<unique_ptr<Expression>> select_list;

public:
	unique_ptr<GlobalOperatorState> GetGlobalOperatorState(ClientContext &context) const override;

	OperatorResultType Execute(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                           GlobalOperatorState &gstate, OperatorState &state) const override;

	OperatorFinalizeResultType FinalExecute(ExecutionContext &context, DataChunk &chunk, GlobalOperatorState &gstate,
	                                        OperatorState &state) const final;

	bool RequiresFinalExecute() const final {
		return true;
	}

	OrderPreservationType OperatorOrder() const override {
		return OrderPreservationType::FIXED_ORDER;
	}

	InsertionOrderPreservingMap<string> ParamsToString() const override;

private:
	void ExecuteFunctions(ExecutionContext &context, DataChunk &chunk, DataChunk &delayed,
	                      GlobalOperatorState &gstate_p) const;
	void ExecuteInput(ExecutionContext &context, DataChunk &delayed, DataChunk &input, DataChunk &chunk,
	                  GlobalOperatorState &gstate) const;
	void ExecuteDelayed(ExecutionContext &context, DataChunk &delayed, DataChunk &input, DataChunk &chunk,
	                    GlobalOperatorState &gstate) const;
	void ExecuteShifted(ExecutionContext &context, DataChunk &delayed, DataChunk &input, DataChunk &chunk,
	                    GlobalOperatorState &gstate) const;
};

} // namespace goose
