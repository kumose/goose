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
