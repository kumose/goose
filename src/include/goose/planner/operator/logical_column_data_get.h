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

#include <goose/common/types/column/column_data_collection.h>
#include <goose/planner/logical_operator.h>
#include <goose/common/optionally_owned_ptr.h>

namespace goose {

class ManagedResultSet;

//! LogicalColumnDataGet represents a scan operation from a ColumnDataCollection
class LogicalColumnDataGet : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_CHUNK_GET;

public:
	LogicalColumnDataGet(idx_t table_index, vector<LogicalType> types, unique_ptr<ColumnDataCollection> collection);
	LogicalColumnDataGet(idx_t table_index, vector<LogicalType> types, ColumnDataCollection &to_scan);
	LogicalColumnDataGet(idx_t table_index, vector<LogicalType> types,
	                     optionally_owned_ptr<ColumnDataCollection> to_scan);

	//! The table index in the current bind context
	idx_t table_index;
	//! The types of the chunk
	vector<LogicalType> chunk_types;
	//! (optionally owned) column data collection
	optionally_owned_ptr<ColumnDataCollection> collection;

public:
	vector<ColumnBinding> GetColumnBindings() override;

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
