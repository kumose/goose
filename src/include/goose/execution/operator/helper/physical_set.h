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

//! PhysicalSet represents a SET operation (e.g. SET a = 42)
class PhysicalSet : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::SET;

public:
	PhysicalSet(PhysicalPlan &physical_plan, const string &name_p, Value value_p, SetScope scope_p,
	            idx_t estimated_cardinality)
	    : PhysicalOperator(physical_plan, PhysicalOperatorType::SET, {LogicalType::BOOLEAN}, estimated_cardinality),
	      name(physical_plan.ArenaRef().MakeString(name_p)), value(std::move(value_p)), scope(scope_p) {
	}

public:
	// Source interface
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

	static void SetExtensionVariable(ClientContext &context, ExtensionOption &extension_option, const String &name,
	                                 SetScope scope, const Value &value);

	static void SetGenericVariable(ClientContext &context, const String &name, SetScope scope, Value target_value);

public:
	String name;
	const Value value;
	const SetScope scope;
};

} // namespace goose
