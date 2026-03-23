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

#include <goose/execution/index/art/art.h>
#include <goose/execution/physical_operator.h>
#include <goose/parser/parsed_data/create_index_info.h>
#include <goose/storage/data_table.h>

#include <fstream>

namespace goose {

class GooseTableEntry;

//! Physical index creation operator.
class PhysicalCreateARTIndex : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::CREATE_INDEX;

public:
	PhysicalCreateARTIndex(PhysicalPlan &physical_plan, LogicalOperator &op, TableCatalogEntry &table,
	                       const vector<column_t> &column_ids, unique_ptr<CreateIndexInfo> info,
	                       vector<unique_ptr<Expression>> unbound_expressions, idx_t estimated_cardinality,
	                       const bool sorted, unique_ptr<AlterTableInfo> alter_table_info = nullptr);

	//! The table to create the index for.
	GooseTableEntry &table;
	//! The list of column IDs of the index.
	vector<column_t> storage_ids;
	//! Index creation information.
	unique_ptr<CreateIndexInfo> info;
	//! Unbound expressions of the indexed columns.
	vector<unique_ptr<Expression>> unbound_expressions;
	//! True, if the pipeline sorts the index data prior to index creation.
	const bool sorted;
	//! Alter table information for adding indexes.
	unique_ptr<AlterTableInfo> alter_table_info;

public:
	//! Source interface, NOP for this operator
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

public:
	//! Sink interface, thread-local sink states. Contains an index for each state.
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	//! Sink interface, global sink state. Contains the global index.
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;

	//! Sink for unsorted data: insert iteratively.
	SinkResultType SinkUnsorted(OperatorSinkInput &input) const;
	//! Sink for sorted data: build + merge.
	SinkResultType SinkSorted(OperatorSinkInput &input) const;

	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	SinkFinalizeType Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
	                          OperatorSinkFinalizeInput &input) const override;

	bool IsSink() const override {
		return true;
	}
	bool ParallelSink() const override {
		return true;
	}
};
} // namespace goose
