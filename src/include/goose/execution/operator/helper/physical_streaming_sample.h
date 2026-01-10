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

//! PhysicalStreamingSample represents a streaming sample using either system or bernoulli sampling
class PhysicalStreamingSample : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::STREAMING_SAMPLE;

public:
	PhysicalStreamingSample(PhysicalPlan &physical_plan, vector<LogicalType> types, unique_ptr<SampleOptions> options,
	                        idx_t estimated_cardinality);

	unique_ptr<SampleOptions> sample_options;
	double percentage;

public:
	// Operator interface
	unique_ptr<OperatorState> GetOperatorState(ExecutionContext &context) const override;
	OperatorResultType Execute(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                           GlobalOperatorState &gstate, OperatorState &state) const override;

	bool ParallelOperator() const override;

	InsertionOrderPreservingMap<string> ParamsToString() const override;

private:
	void SystemSample(DataChunk &input, DataChunk &result, OperatorState &state) const;
	void BernoulliSample(DataChunk &input, DataChunk &result, OperatorState &state) const;
};

} // namespace goose
