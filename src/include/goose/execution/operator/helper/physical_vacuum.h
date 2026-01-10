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
