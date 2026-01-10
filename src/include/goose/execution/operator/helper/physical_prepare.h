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
#include <goose/common/enums/physical_operator_type.h>
#include <goose/main/prepared_statement_data.h>
#include <goose/execution/physical_plan_generator.h>

namespace goose {

class PhysicalPrepare : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::PREPARE;

public:
	PhysicalPrepare(PhysicalPlan &physical_plan, const std::string &name_p, shared_ptr<PreparedStatementData> prepared,
	                idx_t estimated_cardinality)
	    : PhysicalOperator(physical_plan, PhysicalOperatorType::PREPARE, {LogicalType::BOOLEAN}, estimated_cardinality),
	      name(physical_plan.ArenaRef().MakeString(name_p)), prepared(std::move(prepared)) {
	}

	String name;
	shared_ptr<PreparedStatementData> prepared;

public:
	// Source interface
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}
};

} // namespace goose
