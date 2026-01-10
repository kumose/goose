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

namespace goose {

//! LogicalDummyScan represents a dummy scan returning a single row
class LogicalDummyScan : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_DUMMY_SCAN;

public:
	explicit LogicalDummyScan(idx_t table_index)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_DUMMY_SCAN), table_index(table_index) {
	}

	idx_t table_index;

public:
	vector<ColumnBinding> GetColumnBindings() override {
		return {ColumnBinding(table_index, 0)};
	}

	idx_t EstimateCardinality(ClientContext &context) override {
		return 1;
	}
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
	vector<idx_t> GetTableIndex() const override;
	string GetName() const override;

protected:
	void ResolveTypes() override {
		if (types.empty()) {
			types.emplace_back(LogicalType::INTEGER);
		}
	}
};
} // namespace goose
