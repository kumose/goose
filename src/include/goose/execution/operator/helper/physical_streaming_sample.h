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
