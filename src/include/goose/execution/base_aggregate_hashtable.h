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

#include <goose/common/common.h>
#include <goose/common/types/row/tuple_data_layout.h>
#include <goose/common/types/vector.h>
#include <goose/execution/operator/aggregate/aggregate_object.h>

namespace goose {
class BufferManager;

class BaseAggregateHashTable {
public:
	BaseAggregateHashTable(ClientContext &context, Allocator &allocator, const vector<AggregateObject> &aggregates,
	                       vector<LogicalType> payload_types);
	virtual ~BaseAggregateHashTable() {
	}

protected:
	Allocator &allocator;
	BufferManager &buffer_manager;
	//! A helper for managing offsets into the data buffers
	shared_ptr<TupleDataLayout> layout_ptr;
	//! The types of the payload columns stored in the hashtable
	vector<LogicalType> payload_types;
	//! Intermediate structures and data for aggregate filters
	AggregateFilterDataSet filter_set;
};

} // namespace goose
