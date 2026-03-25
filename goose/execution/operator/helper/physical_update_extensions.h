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
#include <goose/parser/parsed_data/update_extensions_info.h>
#include <goose/main/extension_helper.h>

namespace goose {

struct UpdateExtensionsGlobalState : public GlobalSourceState {
	UpdateExtensionsGlobalState() : offset(0) {
	}

	vector<ExtensionUpdateResult> update_result_entries;
	idx_t offset;
};

//! PhysicalUpdateExtensions represents an extension UPDATE operation
class PhysicalUpdateExtensions : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::UPDATE_EXTENSIONS;

public:
	explicit PhysicalUpdateExtensions(PhysicalPlan &physical_plan, unique_ptr<UpdateExtensionsInfo> info,
	                                  idx_t estimated_cardinality)
	    : PhysicalOperator(physical_plan, PhysicalOperatorType::UPDATE_EXTENSIONS,
	                       {LogicalType::VARCHAR, LogicalType::VARCHAR, LogicalType::VARCHAR, LogicalType::VARCHAR,
	                        LogicalType::VARCHAR},
	                       estimated_cardinality),
	      info(std::move(info)) {
	}

	unique_ptr<UpdateExtensionsInfo> info;

	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;

public:
	// Source interface
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}
};

} // namespace goose
