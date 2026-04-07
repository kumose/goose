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
#include <goose/common/optional_idx.h>
#include <goose/common/types/value.h>
#include <goose/common/encryption_state.h>

namespace goose {

enum class CompressInMemory { AUTOMATIC, COMPRESS, DO_NOT_COMPRESS };

struct StorageOptions {
	//! The allocation size of blocks for this attached database file (if any)
	optional_idx block_alloc_size;
	//! The row group size for this attached database (if any)
	optional_idx row_group_size;
	//! Target storage version (if any)
	optional_idx storage_version;
	//! Block header size (only used for encryption)
	optional_idx block_header_size;

	CompressInMemory compress_in_memory = CompressInMemory::AUTOMATIC;

	//! Whether the database is encrypted
	bool encryption = false;
	//! Encryption algorithm
	EncryptionTypes::CipherType encryption_cipher = EncryptionTypes::INVALID;
	//! encryption key
	//! FIXME: change to a unique_ptr in the future
	shared_ptr<string> user_key;

	void Initialize(const unordered_map<string, Value> &options);
};

} // namespace goose
