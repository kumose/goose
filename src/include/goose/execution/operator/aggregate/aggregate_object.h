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

#include <goose/execution/expression_executor.h>
#include <goose/function/aggregate_function.h>

namespace goose {

class BoundAggregateExpression;
class BoundWindowExpression;

struct FunctionDataWrapper {
	explicit FunctionDataWrapper(unique_ptr<FunctionData> function_data_p) : function_data(std::move(function_data_p)) {
	}

	unique_ptr<FunctionData> function_data;
};

struct AggregateObject { // NOLINT: work-around bug in clang-tidy
	AggregateObject(AggregateFunction function, FunctionData *bind_data, idx_t child_count, idx_t payload_size,
	                AggregateType aggr_type, PhysicalType return_type, Expression *filter = nullptr);
	explicit AggregateObject(BoundAggregateExpression *aggr);
	explicit AggregateObject(const BoundWindowExpression &window);

	FunctionData *GetFunctionData() const {
		return bind_data_wrapper ? bind_data_wrapper->function_data.get() : nullptr;
	}

	AggregateFunction function;
	shared_ptr<FunctionDataWrapper> bind_data_wrapper;
	idx_t child_count;
	idx_t payload_size;
	AggregateType aggr_type;
	PhysicalType return_type;
	Expression *filter = nullptr;

public:
	bool IsDistinct() const {
		return aggr_type == AggregateType::DISTINCT;
	}
	static vector<AggregateObject> CreateAggregateObjects(const vector<BoundAggregateExpression *> &bindings);
};

struct AggregateFilterData {
	AggregateFilterData(ClientContext &context, Expression &filter_expr, const vector<LogicalType> &payload_types);

	idx_t ApplyFilter(DataChunk &payload);

	ExpressionExecutor filter_executor;
	DataChunk filtered_payload;
	SelectionVector true_sel;
};

struct AggregateFilterDataSet {
	AggregateFilterDataSet();

	vector<unique_ptr<AggregateFilterData>> filter_data;

public:
	void Initialize(ClientContext &context, const vector<AggregateObject> &aggregates,
	                const vector<LogicalType> &payload_types);

	AggregateFilterData &GetFilterData(idx_t aggr_idx);
};

} // namespace goose
