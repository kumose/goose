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

#include <goose/common/encryption_state.h>
#include <goose/common/optional_ptr.h>
#include <goose/common/typedefs.h>

#include <string>

typedef struct mbedtls_cipher_context_t mbedtls_cipher_context_t;
typedef struct mbedtls_cipher_info_t mbedtls_cipher_info_t;

namespace goose_mbedtls {



class MbedTlsWrapper {
public:
	static void ComputeSha256Hash(const char *in, size_t in_len, char *out);
	static std::string ComputeSha256Hash(const std::string &file_content);
	static bool IsValidSha256Signature(const std::string &pubkey, const std::string &signature,
	                                   const std::string &sha256_hash);
	static void Hmac256(const char *key, size_t key_len, const char *message, size_t message_len, char *out);
	static void ToBase16(char *in, char *out, size_t len);

	static constexpr size_t SHA256_HASH_LENGTH_BYTES = 32;
	static constexpr size_t SHA256_HASH_LENGTH_TEXT = 64;

	class SHA256State {
	public:
		SHA256State();
		~SHA256State();
		void AddString(const std::string &str);
		void AddBytes(goose::data_ptr_t input_bytes, goose::idx_t len);
		void AddBytes(goose::const_data_ptr_t input_bytes, goose::idx_t len);
		void AddSalt(unsigned char *salt, size_t salt_len);
		std::string Finalize();
		void FinishHex(char *out);
		void FinalizeDerivedKey(goose::data_ptr_t hash);

	private:
		void *sha_context;
	};

	static constexpr size_t SHA1_HASH_LENGTH_BYTES = 20;
	static constexpr size_t SHA1_HASH_LENGTH_TEXT = 40;

	class SHA1State {
	public:
		SHA1State();
		~SHA1State();
		void AddString(const std::string &str);
		std::string Finalize();
		void FinishHex(char *out);

	private:
		void *sha_context;
	};

class AESStateMBEDTLS : public goose::EncryptionState {
	public:
		GOOSE_API explicit AESStateMBEDTLS(goose::EncryptionTypes::CipherType cipher_p, goose::idx_t key_len);
		GOOSE_API ~AESStateMBEDTLS() override;

	public:
		GOOSE_API void InitializeEncryption(goose::const_data_ptr_t iv, goose::idx_t iv_len, goose::const_data_ptr_t key, goose::idx_t key_len, goose::const_data_ptr_t aad, goose::idx_t aad_len) override;
		GOOSE_API void InitializeDecryption(goose::const_data_ptr_t iv, goose::idx_t iv_len, goose::const_data_ptr_t key, goose::idx_t key_len, goose::const_data_ptr_t aad, goose::idx_t aad_len) override;

		GOOSE_API size_t Process(goose::const_data_ptr_t in, goose::idx_t in_len, goose::data_ptr_t out,
		                          goose::idx_t out_len) override;
		GOOSE_API size_t Finalize(goose::data_ptr_t out, goose::idx_t out_len, goose::data_ptr_t tag, goose::idx_t tag_len) override;

		GOOSE_API static void GenerateRandomDataStatic(goose::data_ptr_t data, goose::idx_t len);
		GOOSE_API void GenerateRandomData(goose::data_ptr_t data, goose::idx_t len) override;
		GOOSE_API void FinalizeGCM(goose::data_ptr_t tag, goose::idx_t tag_len);
		GOOSE_API const mbedtls_cipher_info_t *GetCipher(size_t key_len);
		GOOSE_API static void SecureClearData(goose::data_ptr_t data, goose::idx_t len);

	private:
		GOOSE_API void InitializeInternal(goose::const_data_ptr_t iv, goose::idx_t iv_len, goose::const_data_ptr_t aad, goose::idx_t aad_len);

	private:
		goose::EncryptionTypes::Mode mode;
		goose::unique_ptr<mbedtls_cipher_context_t> context;
	};

	class AESStateMBEDTLSFactory : public goose::EncryptionUtil {

	public:
		goose::shared_ptr<goose::EncryptionState> CreateEncryptionState(goose::EncryptionTypes::CipherType cipher_p, goose::idx_t key_len = 0) const override {
			return goose::make_shared_ptr<MbedTlsWrapper::AESStateMBEDTLS>(cipher_p, key_len);
		}

		~AESStateMBEDTLSFactory() override {} //

		GOOSE_API bool SupportsEncryption() override {
			return false;
		}
	};
};

} // namespace goose_mbedtls
