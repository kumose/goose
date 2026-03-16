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
