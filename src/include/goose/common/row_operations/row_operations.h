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
