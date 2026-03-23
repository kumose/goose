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
