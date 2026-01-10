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
#include <goose/function/table_function.h>
#include <goose/planner/table_filter.h>
#include <goose/storage/data_table.h>

namespace goose {

//! Represents a scan of a base table
class PhysicalPositionalScan : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::POSITIONAL_SCAN;

public:
	//! Regular Table Scan
	PhysicalPositionalScan(PhysicalPlan &physical_plan, vector<LogicalType> types, PhysicalOperator &left,
	                       PhysicalOperator &right);

	//! The child table functions
	vector<reference<PhysicalOperator>> child_tables;

public:
	bool Equals(const PhysicalOperator &other) const override;
	vector<const_reference<PhysicalOperator>> GetChildren() const override;

public:
	unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context,
	                                                 GlobalSourceState &gstate) const override;
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	ProgressData GetProgress(ClientContext &context, GlobalSourceState &gstate) const override;

	bool IsSource() const override {
		return true;
	}
};

} // namespace goose
