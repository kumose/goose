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
