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

#include <goose/common/enums/set_scope.h>
#include <goose/execution/physical_operator.h>
#include <goose/parser/parsed_data/vacuum_info.h>
#include <goose/execution/physical_plan_generator.h>

namespace goose {

struct DBConfig;
struct ExtensionOption;

//! PhysicalReset represents a RESET operation (e.g. RESET a = 42)
class PhysicalReset : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::RESET;

public:
	PhysicalReset(PhysicalPlan &physical_plan, const std::string &name_p, SetScope scope_p, idx_t estimated_cardinality)
	    : PhysicalOperator(physical_plan, PhysicalOperatorType::RESET, {LogicalType::BOOLEAN}, estimated_cardinality),
	      name(physical_plan.ArenaRef().MakeString(name_p)), scope(scope_p) {
	}

public:
	// Source interface
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

public:
	const String name;
	const SetScope scope;

private:
	void ResetExtensionVariable(ExecutionContext &context, DBConfig &config, ExtensionOption &extension_option) const;
};

} // namespace goose
