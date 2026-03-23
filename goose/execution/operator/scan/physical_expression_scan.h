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
#include <goose/planner/expression.h>

namespace goose {

//! The PhysicalExpressionScan scans a set of expressions
class PhysicalExpressionScan : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::EXPRESSION_SCAN;

public:
	PhysicalExpressionScan(PhysicalPlan &physical_plan, vector<LogicalType> types,
	                       vector<vector<unique_ptr<Expression>>> expressions, idx_t estimated_cardinality)
	    : PhysicalOperator(physical_plan, PhysicalOperatorType::EXPRESSION_SCAN, std::move(types),
	                       estimated_cardinality),
	      expressions(std::move(expressions)) {
	}

	//! The set of expressions to scan
	vector<vector<unique_ptr<Expression>>> expressions;

public:
	unique_ptr<OperatorState> GetOperatorState(ExecutionContext &context) const override;
	OperatorResultType Execute(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                           GlobalOperatorState &gstate, OperatorState &state) const override;

	bool ParallelOperator() const override {
		return true;
	}

public:
	bool IsFoldable() const;
	void EvaluateExpression(ClientContext &context, idx_t expression_idx, optional_ptr<DataChunk> child_chunk,
	                        DataChunk &result, optional_ptr<DataChunk> temp_chunk_ptr = nullptr) const;

private:
	void EvaluateExpressionInternal(ClientContext &context, idx_t expression_idx, optional_ptr<DataChunk> child_chunk,
	                                DataChunk &result, DataChunk &temp_chunk) const;
};

} // namespace goose
