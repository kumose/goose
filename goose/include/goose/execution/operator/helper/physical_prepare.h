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
