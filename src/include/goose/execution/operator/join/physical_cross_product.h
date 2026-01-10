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

//! PhysicalCrossProduct represents a cross product between two tables
class PhysicalCrossProduct : public CachingPhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::CROSS_PRODUCT;

public:
	PhysicalCrossProduct(PhysicalPlan &physical_plan, vector<LogicalType> types, PhysicalOperator &left,
	                     PhysicalOperator &right, idx_t estimated_cardinality);

public:
	// Operator Interface
	unique_ptr<OperatorState> GetOperatorState(ExecutionContext &context) const override;

	OrderPreservationType OperatorOrder() const override {
		return OrderPreservationType::NO_ORDER;
	}
	bool ParallelOperator() const override {
		return true;
	}

protected:
	OperatorResultType ExecuteInternal(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                                   GlobalOperatorState &gstate, OperatorState &state) const override;

public:
	// Sink Interface
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;

	bool IsSink() const override {
		return true;
	}
	bool ParallelSink() const override {
		return true;
	}
	bool SinkOrderDependent() const override {
		return false;
	}

public:
	void BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) override;
	vector<const_reference<PhysicalOperator>> GetSources() const override;
};

class CrossProductExecutor {
public:
	explicit CrossProductExecutor(ColumnDataCollection &rhs);

	OperatorResultType Execute(DataChunk &input, DataChunk &output);

	// returns if the left side is scanned as a constant vector
	bool ScanLHS() {
		return scan_input_chunk;
	}

	// returns the position in the chunk of chunk scanned as a constant input vector
	idx_t PositionInChunk() {
		return position_in_chunk;
	}

	idx_t ScanPosition() {
		return scan_state.current_row_index;
	}

private:
	void Reset(DataChunk &input, DataChunk &output);
	bool NextValue(DataChunk &input, DataChunk &output);

private:
	ColumnDataCollection &rhs;
	ColumnDataScanState scan_state;
	DataChunk scan_chunk;
	idx_t position_in_chunk;
	bool initialized;
	bool finished;
	bool scan_input_chunk;
};

} // namespace goose
