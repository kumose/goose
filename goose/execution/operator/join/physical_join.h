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
#include <goose/planner/operator/logical_comparison_join.h>

namespace goose {

//! PhysicalJoin represents the base class of the join operators
class PhysicalJoin : public CachingPhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::INVALID;

public:
	PhysicalJoin(PhysicalPlan &physical_plan, LogicalOperator &op, PhysicalOperatorType type, JoinType join_type,
	             idx_t estimated_cardinality);

	JoinType join_type;

public:
	bool EmptyResultIfRHSIsEmpty() const;

	static bool HasNullValues(DataChunk &chunk);
	static void ConstructSemiJoinResult(DataChunk &left, DataChunk &result, bool found_match[]);
	static void ConstructAntiJoinResult(DataChunk &left, DataChunk &result, bool found_match[]);
	static void ConstructMarkJoinResult(DataChunk &join_keys, DataChunk &left, DataChunk &result, bool found_match[],
	                                    bool has_null);

public:
	static void BuildJoinPipelines(Pipeline &current, MetaPipeline &meta_pipeline, PhysicalOperator &op,
	                               bool build_rhs = true);
	void BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) override;
	vector<const_reference<PhysicalOperator>> GetSources() const override;

	OrderPreservationType SourceOrder() const override {
		return OrderPreservationType::NO_ORDER;
	}
	OrderPreservationType OperatorOrder() const override {
		return OrderPreservationType::NO_ORDER;
	}
	bool SinkOrderDependent() const override {
		return false;
	}
};

} // namespace goose
