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

#include <goose/common/optionally_owned_ptr.h>
#include <goose/common/types/column/column_data_collection.h>
#include <goose/execution/physical_operator.h>

namespace goose {

//! The PhysicalColumnDataScan scans a ColumnDataCollection
class PhysicalColumnDataScan : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::INVALID;

public:
	PhysicalColumnDataScan(PhysicalPlan &physical_plan, vector<LogicalType> types, PhysicalOperatorType op_type,
	                       idx_t estimated_cardinality, optionally_owned_ptr<ColumnDataCollection> collection);

	PhysicalColumnDataScan(PhysicalPlan &physical_plan, vector<LogicalType> types, PhysicalOperatorType op_type,
	                       idx_t estimated_cardinality, idx_t cte_index);

	//! (optionally owned) column data collection to scan
	optionally_owned_ptr<ColumnDataCollection> collection;

	idx_t cte_index;
	optional_idx delim_index;

public:
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context,
	                                                 GlobalSourceState &gstate) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

	InsertionOrderPreservingMap<string> ParamsToString() const override;
	bool ParallelSource() const override {
		return true;
	}

public:
	void BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) override;
};

} // namespace goose
