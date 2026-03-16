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
