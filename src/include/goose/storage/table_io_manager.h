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
