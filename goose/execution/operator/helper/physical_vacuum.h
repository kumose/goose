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

#include <goose/common/types-import.h>
#include <goose/execution/physical_operator.h>
#include <goose/parser/parsed_data/vacuum_info.h>

namespace goose {

//! PhysicalVacuum represents a VACUUM operation (i.e. VACUUM or ANALYZE)
class PhysicalVacuum : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::VACUUM;

public:
	PhysicalVacuum(PhysicalPlan &physical_plan, unique_ptr<VacuumInfo> info, optional_ptr<TableCatalogEntry> table,
	               unordered_map<idx_t, idx_t> column_id_map, idx_t estimated_cardinality);

	unique_ptr<VacuumInfo> info;
	optional_ptr<TableCatalogEntry> table;
	unordered_map<idx_t, idx_t> column_id_map;

public:
	// Source interface
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

public:
	// Sink interface
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;

	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	SinkFinalizeType Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
	                          OperatorSinkFinalizeInput &input) const override;

	bool IsSink() const override {
		return info->has_table;
	}

	bool ParallelSink() const override {
		return IsSink();
	}
};

} // namespace goose
