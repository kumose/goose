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

#include <goose/common/helper.h>
#include <goose/common/types/value.h>
#include <goose/common/types-import.h>
#include <goose/common/types.h>
#include <goose/storage/object_cache.h>

namespace goose {

class EncryptionKey {
public:
	explicit EncryptionKey(data_ptr_t encryption_key);
	~EncryptionKey();

	EncryptionKey(const EncryptionKey &) = delete;
	EncryptionKey &operator=(const EncryptionKey &) = delete;

	EncryptionKey(EncryptionKey &&) noexcept = default;
	EncryptionKey &operator=(EncryptionKey &&) noexcept = default;

public:
	const_data_ptr_t GetPtr() const {
		return key;
	}

public:
	static void LockEncryptionKey(data_ptr_t key, idx_t key_len = MainHeader::DEFAULT_ENCRYPTION_KEY_LENGTH);
	static void UnlockEncryptionKey(data_ptr_t key, idx_t key_len = MainHeader::DEFAULT_ENCRYPTION_KEY_LENGTH);

private:
	data_t key[MainHeader::DEFAULT_ENCRYPTION_KEY_LENGTH];
};

class EncryptionKeyManager : public ObjectCacheEntry {
public:
	static EncryptionKeyManager &GetInternal(ObjectCache &cache);
	static EncryptionKeyManager &Get(ClientContext &context);
	static EncryptionKeyManager &Get(DatabaseInstance &db);

public:
	void AddKey(const string &key_name, data_ptr_t key);
	bool HasKey(const string &key_name) const;
	void DeleteKey(const string &key_name);
	const_data_ptr_t GetKey(const string &key_name) const;

public:
	static string ObjectType();
	string GetObjectType() override;
	optional_idx GetEstimatedCacheMemory() const override {
		return optional_idx {};
	}

public:
public:
	static void DeriveKey(string &user_key, data_ptr_t salt, data_ptr_t derived_key);
	static void KeyDerivationFunctionSHA256(const_data_ptr_t user_key, idx_t user_key_size, data_ptr_t salt,
	                                        data_ptr_t derived_key);
	static void KeyDerivationFunctionSHA256(data_ptr_t user_key, idx_t user_key_size, data_ptr_t salt,
	                                        data_ptr_t derived_key);
	static string Base64Decode(const string &key);

	//! Generate a (non-cryptographically secure) random key ID
	static string GenerateRandomKeyID();

public:
	//! constants
	static constexpr idx_t KEY_ID_BYTES = 8;
	static constexpr idx_t DERIVED_KEY_LENGTH = 32;

private:
	mutable mutex lock;
	std::unordered_map<std::string, EncryptionKey> derived_keys;
};

} // namespace goose
