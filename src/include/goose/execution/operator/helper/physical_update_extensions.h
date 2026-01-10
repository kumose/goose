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
