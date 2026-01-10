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
#include <goose/function/function.h>
#include <goose/function/table_function.h>

namespace goose {

class PhysicalTableInOutFunction : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::INOUT_FUNCTION;

public:
	PhysicalTableInOutFunction(PhysicalPlan &physical_plan, vector<LogicalType> types, TableFunction function_p,
	                           unique_ptr<FunctionData> bind_data_p, vector<ColumnIndex> column_ids_p,
	                           idx_t estimated_cardinality, vector<column_t> projected_input);

public:
	unique_ptr<OperatorState> GetOperatorState(ExecutionContext &context) const override;
	unique_ptr<GlobalOperatorState> GetGlobalOperatorState(ClientContext &context) const override;
	static void SetOrdinality(DataChunk &chunk, const optional_idx &ordinality_column_idx, const idx_t &ordinality_idx,
	                          const idx_t &ordinality);
	OperatorResultType Execute(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                           GlobalOperatorState &gstate, OperatorState &state) const override;
	OperatorFinalizeResultType FinalExecute(ExecutionContext &context, DataChunk &chunk, GlobalOperatorState &gstate,
	                                        OperatorState &state) const override;

	bool ParallelOperator() const override {
		return true;
	}

	bool RequiresFinalExecute() const override {
		return function.in_out_function_final;
	}

	InsertionOrderPreservingMap<string> ParamsToString() const override;

	//! Information for WITH ORDINALITY
	optional_idx ordinality_idx;

	OrderPreservationType OperatorOrder() const override {
		return function.order_preservation_type;
	}

private:
	//! The table function
	TableFunction function;
	//! Bind data of the function
	unique_ptr<FunctionData> bind_data;
	//! The set of column ids to fetch
	vector<ColumnIndex> column_ids;
	//! The set of input columns to project out
	vector<column_t> projected_input;
};

} // namespace goose
