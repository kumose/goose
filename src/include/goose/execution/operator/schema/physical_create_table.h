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
#include <goose/planner/parsed_data/bound_create_table_info.h>

namespace goose {

//! Physically CREATE TABLE statement
class PhysicalCreateTable : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::CREATE_TABLE;

public:
	PhysicalCreateTable(PhysicalPlan &physical_plan, LogicalOperator &op, SchemaCatalogEntry &schema,
	                    unique_ptr<BoundCreateTableInfo> info, idx_t estimated_cardinality);

	//! Schema to insert to
	SchemaCatalogEntry &schema;
	//! Table name to create
	unique_ptr<BoundCreateTableInfo> info;

public:
	// Source interface
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}
};
} // namespace goose
