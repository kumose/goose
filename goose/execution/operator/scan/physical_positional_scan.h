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
