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
