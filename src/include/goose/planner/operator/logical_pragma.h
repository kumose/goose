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

#include <goose/planner/logical_operator.h>
#include <goose/parser/parsed_data/bound_pragma_info.h>

namespace goose {

//! LogicalPragma represents a simple logical operator that only passes on the parse info
class LogicalPragma : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_PRAGMA;

public:
	explicit LogicalPragma(unique_ptr<BoundPragmaInfo> info_p)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_PRAGMA), info(std::move(info_p)) {
	}

	unique_ptr<BoundPragmaInfo> info;

public:
	idx_t EstimateCardinality(ClientContext &context) override;
	//! Skips the serialization check in VerifyPlan
	bool SupportSerialization() const override {
		return false;
	}

protected:
	void ResolveTypes() override {
		types.emplace_back(LogicalType::BOOLEAN);
	}
};
} // namespace goose
