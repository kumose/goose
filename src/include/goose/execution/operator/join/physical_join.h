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
