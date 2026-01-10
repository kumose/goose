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

#include <goose/main/prepared_statement_data.h>
#include <goose/planner/logical_operator.h>

namespace goose {

class LogicalExecute : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_EXECUTE;

public:
	explicit LogicalExecute(shared_ptr<PreparedStatementData> prepared_p)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_EXECUTE), prepared(std::move(prepared_p)) {
		D_ASSERT(prepared);
		types = prepared->types;
	}

	shared_ptr<PreparedStatementData> prepared;

public:
	//! Skips the serialization check in VerifyPlan
	bool SupportSerialization() const override {
		return false;
	}

protected:
	void ResolveTypes() override {
		types = prepared->types;
	}
	vector<ColumnBinding> GetColumnBindings() override {
		return GenerateColumnBindings(0, types.size());
	}
};
} // namespace goose
