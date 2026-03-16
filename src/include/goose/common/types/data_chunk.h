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

#include <goose/common/allocator.h>
#include <goose/common/arrow/arrow_wrapper.h>
#include <goose/common/common.h>
#include <goose/common/types/vector.h>
#include <goose/common/winapi.h>

namespace goose {
class Allocator;
class ClientContext;
class ExecutionContext;
class VectorCache;
class Serializer;
class Deserializer;

//!  A Data Chunk represents a set of vectors.
/*!
    The data chunk class is the intermediate representation used by the
   execution engine of Goose. It effectively represents a subset of a relation.
   It holds a set of vectors that all have the same length.

    DataChunk is initialized using the DataChunk::Initialize function by
   providing it with a vector of TypeIds for the Vector members. By default,
   this function will also allocate a chunk of memory in the DataChunk for the
   vectors and all the vectors will be referencing vectors to the data owned by
   the chunk. The reason for this behavior is that the underlying vectors can
   become referencing vectors to other chunks as well (i.e. in the case an
   operator does not alter the data, such as a Filter operator which only adds a
   selection vector).

    In addition to holding the data of the vectors, the DataChunk also owns the
   selection vector that underlying vectors can point to.
*/
class DataChunk {
public:
	//! Creates an empty DataChunk
	GOOSE_API DataChunk();
	GOOSE_API ~DataChunk();

	//! The vectors owned by the DataChunk.
	vector<Vector> data;

public:
	inline idx_t size() const { // NOLINT
		return count;
	}
	inline idx_t ColumnCount() const {
		return data.size();
	}
	inline void SetCardinality(idx_t count_p) {
		D_ASSERT(count_p <= capacity);
		this->count = count_p;
	}
	inline void SetCardinality(const DataChunk &other) {
		SetCardinality(other.size());
	}
	inline idx_t GetCapacity() const {
		return capacity;
	}
	inline void SetCapacity(idx_t capacity_p) {
		this->capacity = capacity_p;
	}
	inline void SetCapacity(const DataChunk &other) {
		SetCapacity(other.capacity);
	}

	GOOSE_API Value GetValue(idx_t col_idx, idx_t index) const;
	GOOSE_API void SetValue(idx_t col_idx, idx_t index, const Value &val);

	idx_t GetAllocationSize() const;

	//! Returns true if all vectors in the DataChunk are constant
	GOOSE_API bool AllConstant() const;

	//! Set the DataChunk to reference another data chunk
	GOOSE_API void Reference(DataChunk &chunk);
	//! Set the DataChunk to own the data of data chunk, destroying the other chunk in the process
	GOOSE_API void Move(DataChunk &chunk);

	//! Initializes a DataChunk with the given types and without any vector data allocation.
	GOOSE_API void InitializeEmpty(const vector<LogicalType> &types);

	//! Initializes a DataChunk with the given types. Then, if the corresponding boolean in the initialize-vector is
	//! true, it initializes the vector for that data type.
	GOOSE_API void Initialize(ClientContext &context, const vector<LogicalType> &types,
	                           idx_t capacity = STANDARD_VECTOR_SIZE);
	GOOSE_API void Initialize(Allocator &allocator, const vector<LogicalType> &types,
	                           idx_t capacity = STANDARD_VECTOR_SIZE);
	GOOSE_API void Initialize(ClientContext &context, const vector<LogicalType> &types, const vector<bool> &initialize,
	                           idx_t capacity = STANDARD_VECTOR_SIZE);
	GOOSE_API void Initialize(Allocator &allocator, const vector<LogicalType> &types, const vector<bool> &initialize,
	                           idx_t capacity = STANDARD_VECTOR_SIZE);

	//! Append the other DataChunk to this one. The column count and types of
	//! the two DataChunks have to match exactly. Throws an exception if there
	//! is not enough space in the chunk and resize is not allowed.
	GOOSE_API void Append(const DataChunk &other, bool resize = false, SelectionVector *sel = nullptr,
	                       idx_t count = 0);

	//! Destroy all data and columns owned by this DataChunk
	GOOSE_API void Destroy();

	//! Copies the data from this chunk to another chunk.
	GOOSE_API void Copy(DataChunk &other, idx_t offset = 0) const;
	GOOSE_API void Copy(DataChunk &other, const SelectionVector &sel, const idx_t source_count,
	                     const idx_t offset = 0) const;

	//! Splits the DataChunk in two
	GOOSE_API void Split(DataChunk &other, idx_t split_idx);

	//! Fuses a DataChunk onto the right of this one, and destroys the other. Inverse of Split.
	GOOSE_API void Fuse(DataChunk &other);

	//! Makes this DataChunk reference the specified columns in the other DataChunk
	GOOSE_API void ReferenceColumns(DataChunk &other, const vector<column_t> &column_ids);

	//! Turn all the vectors from the chunk into flat vectors
	GOOSE_API void Flatten();

	// FIXME: this is GOOSE_API, might need conversion back to regular unique ptr?
	GOOSE_API unsafe_unique_array<UnifiedVectorFormat> ToUnifiedFormat();

	GOOSE_API void Slice(const SelectionVector &sel_vector, idx_t count);

	//! Slice all Vectors from other.data[i] to data[i + 'col_offset']
	//! Turning all Vectors into Dictionary Vectors, using 'sel'
	GOOSE_API void Slice(const DataChunk &other, const SelectionVector &sel, idx_t count, idx_t col_offset = 0);

	//! Slice a DataChunk from "offset" to "offset + count"
	GOOSE_API void Slice(idx_t offset, idx_t count);

	//! Resets the DataChunk to its state right after the DataChunk::Initialize
	//! function was called. This sets the count to 0, the capacity to initial_capacity and resets each member
	//! Vector to point back to the data owned by this DataChunk.
	GOOSE_API void Reset();

	GOOSE_API void Serialize(Serializer &serializer, bool compressed_serialization = true) const;
	GOOSE_API void Deserialize(Deserializer &source);

	//! Hashes the DataChunk to the target vector
	GOOSE_API void Hash(Vector &result);
	//! Hashes specific vectors of the DataChunk to the target vector
	GOOSE_API void Hash(vector<idx_t> &column_ids, Vector &result);

	//! Returns a list of types of the vectors of this data chunk
	GOOSE_API vector<LogicalType> GetTypes() const;

	//! Converts this DataChunk to a printable string representation
	GOOSE_API string ToString() const;
	GOOSE_API void Print() const;

	DataChunk(const DataChunk &) = delete;

	//! Verify that the DataChunk is in a consistent, not corrupt state. DEBUG
	//! FUNCTION ONLY!
	GOOSE_API void Verify();

private:
	//! The amount of tuples stored in the data chunk
	idx_t count;
	//! The amount of tuples that can be stored in the data chunk
	idx_t capacity;
	//! The initial capacity of this chunk set during ::Initialize, used when resetting
	idx_t initial_capacity;
	//! Vector caches, used to store data when ::Initialize is called
	vector<VectorCache> vector_caches;
};
} // namespace goose
