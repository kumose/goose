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

#include <goose/common/optionally_owned_ptr.h>
#include <goose/common/types/column/column_data_collection.h>
#include <goose/execution/physical_operator.h>

namespace goose {

//! The PhysicalColumnDataScan scans a ColumnDataCollection
class PhysicalColumnDataScan : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::INVALID;

public:
	PhysicalColumnDataScan(PhysicalPlan &physical_plan, vector<LogicalType> types, PhysicalOperatorType op_type,
	                       idx_t estimated_cardinality, optionally_owned_ptr<ColumnDataCollection> collection);

	PhysicalColumnDataScan(PhysicalPlan &physical_plan, vector<LogicalType> types, PhysicalOperatorType op_type,
	                       idx_t estimated_cardinality, idx_t cte_index);

	//! (optionally owned) column data collection to scan
	optionally_owned_ptr<ColumnDataCollection> collection;

	idx_t cte_index;
	optional_idx delim_index;

public:
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context,
	                                                 GlobalSourceState &gstate) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

	InsertionOrderPreservingMap<string> ParamsToString() const override;
	bool ParallelSource() const override {
		return true;
	}

public:
	void BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) override;
};

} // namespace goose
