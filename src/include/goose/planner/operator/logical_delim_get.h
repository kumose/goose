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

//! LogicalDelimGet represents a duplicate eliminated scan belonging to a DelimJoin
class LogicalDelimGet : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_DELIM_GET;

public:
	LogicalDelimGet(idx_t table_index, vector<LogicalType> types)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_DELIM_GET), table_index(table_index) {
		D_ASSERT(!types.empty());
		chunk_types = std::move(types);
	}

	//! The table index in the current bind context
	idx_t table_index;
	//! The types of the chunk
	vector<LogicalType> chunk_types;

public:
	vector<ColumnBinding> GetColumnBindings() override {
		return GenerateColumnBindings(table_index, chunk_types.size());
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
	vector<idx_t> GetTableIndex() const override;
	string GetName() const override;

protected:
	void ResolveTypes() override {
		// types are resolved in the constructor
		this->types = chunk_types;
	}
};
} // namespace goose
