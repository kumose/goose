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

#include <goose/common/optional_idx.h>
#include <goose/execution/physical_operator.h>
#include <goose/execution/physical_operator_states.h>
#include <goose/function/table_function.h>
#include <goose/planner/table_filter.h>
#include <goose/storage/data_table.h>
#include <goose/common/extra_operator_info.h>
#include <goose/common/column_index.h>
#include <goose/execution/physical_table_scan_enum.h>

namespace goose {

//! Represents a scan of a base table
class PhysicalTableScan : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::TABLE_SCAN;

public:
	//! Table scan that immediately projects out filter columns that are unused in the remainder of the query plan
	PhysicalTableScan(PhysicalPlan &physical_plan, vector<LogicalType> types, TableFunction function,
	                  unique_ptr<FunctionData> bind_data, vector<LogicalType> returned_types,
	                  vector<ColumnIndex> column_ids, vector<idx_t> projection_ids, vector<string> names,
	                  unique_ptr<TableFilterSet> table_filters, idx_t estimated_cardinality,
	                  ExtraOperatorInfo extra_info, vector<Value> parameters, virtual_column_map_t virtual_columns);

	//! The table function
	TableFunction function;
	//! Bind data of the function
	unique_ptr<FunctionData> bind_data;
	//! The types of ALL columns that can be returned by the table function
	vector<LogicalType> returned_types;
	//! The column ids used within the table function
	vector<ColumnIndex> column_ids;
	//! The projected-out column ids
	vector<idx_t> projection_ids;
	//! The names of the columns
	vector<string> names;
	//! The table filters
	unique_ptr<TableFilterSet> table_filters;
	//! Currently stores info related to filters pushed down into MultiFileLists and sample rate pushed down into the
	//! table scan
	ExtraOperatorInfo extra_info;
	//! Parameters
	vector<Value> parameters;
	//! Contains a reference to dynamically generated table filters (through e.g. a join up in the tree)
	shared_ptr<DynamicTableFilterSet> dynamic_filters;
	//! Virtual columns
	virtual_column_map_t virtual_columns;

public:
	string GetName() const override;
	InsertionOrderPreservingMap<string> ParamsToString() const override;

	bool Equals(const PhysicalOperator &other) const override;

	OrderPreservationType SourceOrder() const override {
		return function.order_preservation_type;
	}

public:
	unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context,
	                                                 GlobalSourceState &gstate) const override;
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;
	OperatorPartitionData GetPartitionData(ExecutionContext &context, DataChunk &chunk, GlobalSourceState &gstate,
	                                       LocalSourceState &lstate,
	                                       const OperatorPartitionInfo &partition_info) const override;

	bool IsSource() const override {
		return true;
	}
	bool ParallelSource() const override;

	bool SupportsPartitioning(const OperatorPartitionInfo &partition_info) const override;

	ProgressData GetProgress(ClientContext &context, GlobalSourceState &gstate) const override;

	InsertionOrderPreservingMap<string> ExtraSourceParams(GlobalSourceState &gstate,
	                                                      LocalSourceState &lstate) const override;
	optional_idx GetRowsScanned(GlobalSourceState &gstate_p, LocalSourceState &lstate) const;
};

} // namespace goose
