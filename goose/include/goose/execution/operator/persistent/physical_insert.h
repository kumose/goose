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
#include <goose/planner/parsed_data/bound_create_table_info.h>
#include <goose/common/index_vector.h>
#include <goose/parser/statement/insert_statement.h>
#include <goose/storage/table/append_state.h>
#include <goose/catalog/catalog_entry/goose_table_entry.h>
#include <goose/storage/table/delete_state.h>
#include <goose/storage/optimistic_data_writer.h>

namespace goose {

//===--------------------------------------------------------------------===//
// Sink
//===--------------------------------------------------------------------===//
class InsertGlobalState : public GlobalSinkState {
public:
	explicit InsertGlobalState(ClientContext &context, const vector<LogicalType> &return_types, GooseTableEntry &table);

public:
	mutex lock;
	GooseTableEntry &table;
	idx_t insert_count;
	ColumnDataCollection return_collection;
};

class InsertLocalState : public LocalSinkState {
public:
public:
	InsertLocalState(ClientContext &context, const vector<LogicalType> &types,
	                 const vector<unique_ptr<BoundConstraint>> &bound_constraints);

public:
	ConstraintState &GetConstraintState(DataTable &table, TableCatalogEntry &table_ref);
	TableDeleteState &GetDeleteState(DataTable &table, TableCatalogEntry &table_ref, ClientContext &context);

public:
	//! The chunk containing the tuples that become an update (if DO UPDATE)
	DataChunk update_chunk;
	TableAppendState local_append_state;
	//! An index to the optimistic row group collection vector of the local table storage for this transaction.
	PhysicalIndex collection_index;
	unique_ptr<OptimisticDataWriter> optimistic_writer;
	// Rows that have been updated by a DO UPDATE conflict
	unordered_set<row_t> updated_rows;
	idx_t update_count = 0;
	unique_ptr<ConstraintState> constraint_state;
	const vector<unique_ptr<BoundConstraint>> &bound_constraints;
	//! The delete state for ON CONFLICT handling that is rewritten into DELETE + INSERT.
	unique_ptr<TableDeleteState> delete_state;
	//! The append chunk for ON CONFLICT handling that is rewritting into DELETE + INSERT.
	DataChunk append_chunk;
};

//! Physically insert a set of data into a table
class PhysicalInsert : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::INSERT;

public:
	//! INSERT INTO
	PhysicalInsert(PhysicalPlan &physical_plan, vector<LogicalType> types, TableCatalogEntry &table,
	               vector<unique_ptr<BoundConstraint>> bound_constraints,
	               vector<unique_ptr<Expression>> set_expressions, vector<PhysicalIndex> set_columns,
	               vector<LogicalType> set_types, idx_t estimated_cardinality, bool return_chunk, bool parallel,
	               OnConflictAction action_type, unique_ptr<Expression> on_conflict_condition,
	               unique_ptr<Expression> do_update_condition, unordered_set<column_t> on_conflict_filter,
	               vector<column_t> columns_to_fetch, bool update_is_del_and_insert);
	//! CREATE TABLE AS
	PhysicalInsert(PhysicalPlan &physical_plan, LogicalOperator &op, SchemaCatalogEntry &schema,
	               unique_ptr<BoundCreateTableInfo> info, idx_t estimated_cardinality, bool parallel);

	//! The table to insert into
	optional_ptr<TableCatalogEntry> insert_table;
	//! The insert types
	vector<LogicalType> insert_types;
	//! The bound constraints for the table
	vector<unique_ptr<BoundConstraint>> bound_constraints;
	//! If the returning statement is present, return the whole chunk
	bool return_chunk;
	//! Table schema, in case of CREATE TABLE AS
	optional_ptr<SchemaCatalogEntry> schema;
	//! Create table info, in case of CREATE TABLE AS
	unique_ptr<BoundCreateTableInfo> info;
	//! Whether or not the INSERT can be executed in parallel
	//! This insert is not order preserving if executed in parallel
	bool parallel;
	// Which action to perform on conflict
	OnConflictAction action_type;

	// The DO UPDATE set expressions, if 'action_type' is UPDATE
	vector<unique_ptr<Expression>> set_expressions;
	// Which columns are targeted by the set expressions
	vector<PhysicalIndex> set_columns;
	// The types of the columns targeted by a SET expression
	vector<LogicalType> set_types;

	// Condition for the ON CONFLICT clause
	unique_ptr<Expression> on_conflict_condition;
	// Condition for the DO UPDATE clause
	unique_ptr<Expression> do_update_condition;
	// The column ids to apply the ON CONFLICT on
	unordered_set<column_t> conflict_target;
	//! True, if the INSERT OR REPLACE requires delete + insert.
	bool update_is_del_and_insert;

	// Column ids from the original table to fetch
	vector<StorageIndex> columns_to_fetch;
	// Matching types to the column ids to fetch
	vector<LogicalType> types_to_fetch;

public:
	// Source interface
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

public:
	// Sink interface
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	SinkFinalizeType Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
	                          OperatorSinkFinalizeInput &input) const override;

	bool IsSink() const override {
		return true;
	}

	bool ParallelSink() const override {
		return parallel;
	}

	bool SinkOrderDependent() const override {
		return true;
	}

public:
	static void GetInsertInfo(const BoundCreateTableInfo &info, vector<LogicalType> &insert_types);

protected:
	void CombineExistingAndInsertTuples(DataChunk &result, DataChunk &scan_chunk, DataChunk &input_chunk,
	                                    ClientContext &client) const;
	//! Returns the amount of updated tuples
	void CreateUpdateChunk(ExecutionContext &context, DataChunk &chunk, TableCatalogEntry &table, Vector &row_ids,
	                       DataChunk &result) const;
	idx_t OnConflictHandling(TableCatalogEntry &table, ExecutionContext &context, InsertGlobalState &gstate,
	                         InsertLocalState &lstate, DataChunk &insert_chunk) const;
};

} // namespace goose
