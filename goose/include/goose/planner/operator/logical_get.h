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

#include <goose/function/table_function.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/table_filter.h>
#include <goose/common/extra_operator_info.h>

namespace goose {
class DynamicTableFilterSet;

//! LogicalGet represents a scan operation from a data source
class LogicalGet : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_GET;

public:
	LogicalGet(idx_t table_index, TableFunction function, unique_ptr<FunctionData> bind_data,
	           vector<LogicalType> returned_types, vector<string> returned_names,
	           virtual_column_map_t virtual_columns = virtual_column_map_t());

	//! The table index in the current bind context
	idx_t table_index;
	//! The function that is called
	TableFunction function;
	//! The bind data of the function
	unique_ptr<FunctionData> bind_data;
	//! The types of ALL columns that can be returned by the table function
	vector<LogicalType> returned_types;
	//! The names of ALL columns that can be returned by the table function
	vector<string> names;
	//! A mapping of column index -> type/name for all virtual columns
	virtual_column_map_t virtual_columns;
	//! Columns that are used outside the scan
	vector<idx_t> projection_ids;
	//! Filters pushed down for table scan
	TableFilterSet table_filters;
	//! The set of input parameters for the table function
	vector<Value> parameters;
	//! The set of named input parameters for the table function
	named_parameter_map_t named_parameters;
	//! The set of named input table types for the table-in table-out function
	vector<LogicalType> input_table_types;
	//! The set of named input table names for the table-in table-out function
	vector<string> input_table_names;
	//! For a table-in-out function, the set of projected input columns
	vector<column_t> projected_input;
	//! Currently stores File Filters (as strings) applied by hive partitioning/complex filter pushdown and sample rate
	//! pushed down into the table scan
	//! Stored so the can be included in explain output
	ExtraOperatorInfo extra_info;
	//! Contains a reference to dynamically generated table filters (through e.g. a join up in the tree)
	shared_ptr<DynamicTableFilterSet> dynamic_filters;
	//! Information for WITH ORDINALITY
	optional_idx ordinality_idx;

	string GetName() const override;
	InsertionOrderPreservingMap<string> ParamsToString() const override;
	//! Returns the underlying table that is being scanned, or nullptr if there is none
	optional_ptr<TableCatalogEntry> GetTable() const;
	//! Returns any column to query - preferably the cheapest column
	//! This is used when we are running e.g. a COUNT(*) and don't care about the contents of any columns in the table
	column_t GetAnyColumn() const;

	const LogicalType &GetColumnType(const ColumnIndex &column_index) const;
	const string &GetColumnName(const ColumnIndex &column_index) const;

public:
	void SetColumnIds(vector<ColumnIndex> &&column_ids);
	void AddColumnId(column_t column_id);
	void ClearColumnIds();
	const vector<ColumnIndex> &GetColumnIds() const;
	vector<ColumnIndex> &GetMutableColumnIds();
	vector<ColumnBinding> GetColumnBindings() override;
	idx_t EstimateCardinality(ClientContext &context) override;
	bool TryGetStorageIndex(const ColumnIndex &column_index, StorageIndex &out_index) const;

	vector<idx_t> GetTableIndex() const override;
	//! Skips the serialization check in VerifyPlan
	bool SupportSerialization() const override {
		return function.verify_serialization;
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

protected:
	void ResolveTypes() override;

private:
	LogicalGet();

private:
	//! Bound column IDs
	vector<ColumnIndex> column_ids;
};
} // namespace goose
