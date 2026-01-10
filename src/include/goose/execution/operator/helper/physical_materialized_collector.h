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

#include <goose/execution/operator/helper/physical_result_collector.h>
#include <goose/common/types/column/column_data_scan_states.h>
#include <goose/common/types/column/column_data_collection.h>

namespace goose {

class PhysicalMaterializedCollector : public PhysicalResultCollector {
public:
	PhysicalMaterializedCollector(PhysicalPlan &physical_plan, PreparedStatementData &data, bool parallel);

	bool parallel;

public:
	unique_ptr<QueryResult> GetResult(GlobalSinkState &state) const override;

public:
	// Sink interface
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;

	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;

	bool ParallelSink() const override;
	bool SinkOrderDependent() const override;
};

} // namespace goose
