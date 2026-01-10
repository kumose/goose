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
#include <goose/storage/storage_info.h>

namespace goose {

struct DatabaseSize {
	idx_t total_blocks = 0;
	idx_t block_size = 0;
	idx_t free_blocks = 0;
	idx_t used_blocks = 0;
	idx_t bytes = 0;
	idx_t wal_size = 0;
};

struct MetadataBlockInfo {
	block_id_t block_id;
	idx_t total_blocks;
	vector<idx_t> free_list;
};

} // namespace goose
