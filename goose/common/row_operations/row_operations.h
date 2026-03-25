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

#include <goose/common/enums/expression_type.h>
#include <goose/common/types.h>
#include <goose/common/types/vector.h>

namespace goose {

class ArenaAllocator;
struct AggregateObject;
struct AggregateFilterData;
class DataChunk;
class RowLayout;
class TupleDataLayout;
class RowDataCollection;
struct SelectionVector;
class StringHeap;
struct UnifiedVectorFormat;

struct RowOperationsState {
	explicit RowOperationsState(ArenaAllocator &allocator) : allocator(allocator) {
	}

	ArenaAllocator &allocator;
	unique_ptr<Vector> addresses; // Re-usable vector for row_aggregate.cc
};

// RowOperations contains a set of operations that operate on data using a TupleDataLayout
struct RowOperations {
	//===--------------------------------------------------------------------===//
	// Aggregation Operators
	//===--------------------------------------------------------------------===//
	//! initialize - unaligned addresses
	static void InitializeStates(TupleDataLayout &layout, Vector &addresses, const SelectionVector &sel, idx_t count);
	//! destructor - unaligned addresses, updated
	static void DestroyStates(RowOperationsState &state, TupleDataLayout &layout, Vector &addresses, idx_t count);
	//! update - aligned addresses
	static void UpdateStates(RowOperationsState &state, AggregateObject &aggr, Vector &addresses, DataChunk &payload,
	                         idx_t arg_idx, idx_t count);
	//! filtered update - aligned addresses
	static void UpdateFilteredStates(RowOperationsState &state, AggregateFilterData &filter_data, AggregateObject &aggr,
	                                 Vector &addresses, DataChunk &payload, idx_t arg_idx);
	//! combine - unaligned addresses, updated
	static void CombineStates(RowOperationsState &state, TupleDataLayout &layout, Vector &sources, Vector &targets,
	                          idx_t count);
	//! finalize - unaligned addresses, updated
	static void FinalizeStates(RowOperationsState &state, TupleDataLayout &layout, Vector &addresses, DataChunk &result,
	                           idx_t aggr_idx);
};

} // namespace goose
