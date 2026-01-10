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

#include <goose/common/types/column/column_data_collection.h>
#include <goose/execution/physical_operator.h>

namespace goose {

class RecursiveCTEState;

class PhysicalCTE : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::CTE;

public:
	PhysicalCTE(PhysicalPlan &physical_plan, string ctename, idx_t table_index, vector<LogicalType> types,
	            PhysicalOperator &top, PhysicalOperator &bottom, idx_t estimated_cardinality);
	~PhysicalCTE() override;

	vector<const_reference<PhysicalOperator>> cte_scans;

	shared_ptr<ColumnDataCollection> working_table;

	idx_t table_index;
	string ctename;

public:
	// Sink interface
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;

	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;

	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;

	bool IsSink() const override {
		return true;
	}

	bool ParallelSink() const override {
		return true;
	}

	bool SinkOrderDependent() const override {
		return false;
	}

	InsertionOrderPreservingMap<string> ParamsToString() const override;

public:
	void BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) override;

	vector<const_reference<PhysicalOperator>> GetSources() const override;
};

} // namespace goose
