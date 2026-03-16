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

namespace goose {
class BlockManager;
class DataTable;
class MetadataManager;

class TableIOManager {
public:
	virtual ~TableIOManager() {
	}

	//! Obtains a reference to the TableIOManager of a specific table
	static TableIOManager &Get(DataTable &table);

	//! The block manager used for managing index data
	virtual BlockManager &GetIndexBlockManager() = 0;

	//! The block manager used for storing row group data
	virtual BlockManager &GetBlockManagerForRowData() = 0;

	virtual MetadataManager &GetMetadataManager() = 0;

	//! Returns the target row group size for the table
	virtual idx_t GetRowGroupSize() const = 0;
};

} // namespace goose
