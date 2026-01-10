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
