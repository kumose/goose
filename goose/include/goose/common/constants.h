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

#include <memory>
#include <goose/common/string.h>
#include <goose/common/winapi.h>
#include <goose/common/unique_ptr.h>
#include <goose/common/typedefs.h>

namespace goose {
class Serializer;
class Deserializer;
class BinarySerializer;
class BinaryDeserializer;
class WriteStream;
class ReadStream;

//! inline std directives that we use frequently
#ifndef GOOSE_DEBUG_MOVE
using std::move;
#endif

// NOTE: there is a copy of this in the Postgres' parser grammar (gram.y)
#define DEFAULT_SCHEMA  "main"
#define INVALID_SCHEMA  ""
#define INVALID_CATALOG ""
#define SYSTEM_CATALOG  "system"
#define TEMP_CATALOG    "temp"
#define IN_MEMORY_PATH  ":memory:"

GOOSE_API bool IsInvalidSchema(const string &str);
GOOSE_API bool IsInvalidCatalog(const string &str);

//! Special value used to signify the ROW ID of a table
GOOSE_API extern const column_t COLUMN_IDENTIFIER_ROW_ID;
//! Special value used to signify an empty column (used for e.g. COUNT(*))
GOOSE_API extern const column_t COLUMN_IDENTIFIER_EMPTY;
GOOSE_API extern const column_t VIRTUAL_COLUMN_START;
GOOSE_API bool IsRowIdColumnId(column_t column_id);
GOOSE_API bool IsVirtualColumn(column_t column_id);

//! The maximum row identifier used in tables
extern const row_t MAX_ROW_ID;
//! Transaction-local row IDs start at MAX_ROW_ID
extern const row_t MAX_ROW_ID_LOCAL;

extern const transaction_t TRANSACTION_ID_START;
extern const transaction_t MAX_TRANSACTION_ID;
extern const transaction_t MAXIMUM_QUERY_ID;
extern const transaction_t NOT_DELETED_ID;

extern const double PI;

struct DConstants {
	//! The value used to signify an invalid index entry
	static constexpr const idx_t INVALID_INDEX = idx_t(-1);
	//! The total maximum vector size (128GB)
	static constexpr const idx_t MAX_VECTOR_SIZE = 1ULL << 37ULL;
};

struct LogicalIndex {
	explicit LogicalIndex(idx_t index) : index(index) {
	}

	idx_t index;

	inline bool operator==(const LogicalIndex &rhs) const {
		return index == rhs.index;
	};
	inline bool operator!=(const LogicalIndex &rhs) const {
		return index != rhs.index;
	};
	inline bool operator<(const LogicalIndex &rhs) const {
		return index < rhs.index;
	};
	bool IsValid() {
		return index != DConstants::INVALID_INDEX;
	}
};

struct PhysicalIndex {
	explicit PhysicalIndex(idx_t index) : index(index) {
	}

	idx_t index;

	inline bool operator==(const PhysicalIndex &rhs) const {
		return index == rhs.index;
	};
	inline bool operator!=(const PhysicalIndex &rhs) const {
		return index != rhs.index;
	};
	inline bool operator<(const PhysicalIndex &rhs) const {
		return index < rhs.index;
	};
	bool IsValid() {
		return index != DConstants::INVALID_INDEX;
	}
};

GOOSE_API bool IsPowerOfTwo(uint64_t v);
GOOSE_API uint64_t NextPowerOfTwo(uint64_t v);
GOOSE_API uint64_t PreviousPowerOfTwo(uint64_t v);

} // namespace goose
