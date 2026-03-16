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
#include <goose/common/string_util.h>

namespace goose {

class EncryptionTypes {
public:
	enum CipherType : uint8_t { INVALID = 0, GCM = 1, CTR = 2, CBC = 3 };
	enum KeyDerivationFunction : uint8_t { DEFAULT = 0, SHA256 = 1, PBKDF2 = 2 };
	enum Mode { ENCRYPT, DECRYPT };

	static string CipherToString(CipherType cipher_p);
	static CipherType StringToCipher(const string &encryption_cipher_p);
	static string KDFToString(KeyDerivationFunction kdf_p);
	static KeyDerivationFunction StringToKDF(const string &key_derivation_function_p);
};

class EncryptionState {
public:
	GOOSE_API explicit EncryptionState(EncryptionTypes::CipherType cipher_p, idx_t key_len);
	GOOSE_API virtual ~EncryptionState();

public:
	GOOSE_API virtual void InitializeEncryption(const_data_ptr_t iv, idx_t iv_len, const_data_ptr_t key, idx_t key_len,
	                                             const_data_ptr_t aad = nullptr, idx_t aad_len = 0);
	GOOSE_API virtual void InitializeDecryption(const_data_ptr_t iv, idx_t iv_len, const_data_ptr_t key, idx_t key_len,
	                                             const_data_ptr_t aad = nullptr, idx_t aad_len = 0);
	GOOSE_API virtual size_t Process(const_data_ptr_t in, idx_t in_len, data_ptr_t out, idx_t out_len);
	GOOSE_API virtual size_t Finalize(data_ptr_t out, idx_t out_len, data_ptr_t tag, idx_t tag_len);
	GOOSE_API virtual void GenerateRandomData(data_ptr_t data, idx_t len);

protected:
	EncryptionTypes::CipherType cipher;
	idx_t key_len;
};

class EncryptionUtil {
public:
	GOOSE_API explicit EncryptionUtil() {};

public:
	virtual shared_ptr<EncryptionState> CreateEncryptionState(EncryptionTypes::CipherType cipher_p,
	                                                          idx_t key_len = 0) const {
		return make_shared_ptr<EncryptionState>(cipher_p, key_len);
	}

	virtual ~EncryptionUtil() {
	}

	//! Whether the EncryptionUtil supports encryption (some may only support decryption)
	GOOSE_API virtual bool SupportsEncryption() {
		return true;
	}
};

} // namespace goose
