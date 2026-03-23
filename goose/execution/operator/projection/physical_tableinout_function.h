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
