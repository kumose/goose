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
