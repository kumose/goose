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

namespace goose {

class PhysicalHashAggregate;

//! PhysicalDelimJoin represents a join where either the LHS or RHS will be duplicate eliminated and pushed into a
//! PhysicalColumnDataScan in the other side. Implementations are PhysicalLeftDelimJoin and PhysicalRightDelimJoin
class PhysicalDelimJoin : public PhysicalOperator {
public:
	PhysicalDelimJoin(PhysicalPlan &physical_plan, PhysicalOperatorType type, vector<LogicalType> types,
	                  PhysicalOperator &original_join, PhysicalOperator &distinct,
	                  const vector<const_reference<PhysicalOperator>> &delim_scans, idx_t estimated_cardinality,
	                  optional_idx delim_idx);

	PhysicalOperator &join;
	PhysicalHashAggregate &distinct;
	vector<const_reference<PhysicalOperator>> delim_scans;

	optional_idx delim_idx;

public:
	vector<const_reference<PhysicalOperator>> GetChildren() const override;

	bool IsSink() const override {
		return true;
	}
	bool ParallelSink() const override {
		return true;
	}
	OrderPreservationType SourceOrder() const override {
		return OrderPreservationType::NO_ORDER;
	}
	bool SinkOrderDependent() const override {
		return false;
	}

	InsertionOrderPreservingMap<string> ParamsToString() const override;
};

} // namespace goose
