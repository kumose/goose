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

#include <goose/execution/physical_operator.h>
#include <goose/parser/parsed_data/sample_options.h>

namespace goose {

//! PhysicalReservoirSample represents a sample taken using reservoir sampling,
//! which is a blocking sampling method

class PhysicalReservoirSample : public PhysicalOperator {
public:
	PhysicalReservoirSample(PhysicalPlan &physical_plan, vector<LogicalType> types, unique_ptr<SampleOptions> options,
	                        idx_t estimated_cardinality)
	    : PhysicalOperator(physical_plan, PhysicalOperatorType::RESERVOIR_SAMPLE, std::move(types),
	                       estimated_cardinality),
	      options(std::move(options)) {
	}

	unique_ptr<SampleOptions> options;

public:
	// Source interface
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

public:
	// Sink interface
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	SinkFinalizeType Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
	                          OperatorSinkFinalizeInput &input) const override;
	bool ParallelSink() const override {
		return !options->repeatable;
	}

	bool IsSink() const override {
		return true;
	}

	InsertionOrderPreservingMap<string> ParamsToString() const override;
};

} // namespace goose
