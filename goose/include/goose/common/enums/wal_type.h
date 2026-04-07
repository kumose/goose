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

#include <goose/common/constants.h>

namespace goose {

enum class WALType : uint8_t {
	INVALID = 0,
	// -----------------------------
	// Catalog
	// -----------------------------
	CREATE_TABLE = 1,
	DROP_TABLE = 2,

	CREATE_SCHEMA = 3,
	DROP_SCHEMA = 4,

	CREATE_VIEW = 5,
	DROP_VIEW = 6,

	CREATE_SEQUENCE = 8,
	DROP_SEQUENCE = 9,
	SEQUENCE_VALUE = 10,

	CREATE_MACRO = 11,
	DROP_MACRO = 12,

	CREATE_TYPE = 13,
	DROP_TYPE = 14,

	ALTER_INFO = 20,

	CREATE_TABLE_MACRO = 21,
	DROP_TABLE_MACRO = 22,

	CREATE_INDEX = 23,
	DROP_INDEX = 24,

	// -----------------------------
	// Data
	// -----------------------------
	USE_TABLE = 25,
	INSERT_TUPLE = 26,
	DELETE_TUPLE = 27,
	UPDATE_TUPLE = 28,
	ROW_GROUP_DATA = 29,
	// -----------------------------
	// Flush
	// -----------------------------
	WAL_VERSION = 98,
	CHECKPOINT = 99,
	WAL_FLUSH = 100
};
}
