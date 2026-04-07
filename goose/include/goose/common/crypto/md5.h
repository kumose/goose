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
#include <goose/common/types/string_type.h>

namespace goose {

class MD5Context {
public:
	static constexpr idx_t MD5_HASH_LENGTH_BINARY = 16;
	static constexpr idx_t MD5_HASH_LENGTH_TEXT = 32;

public:
	MD5Context();

	void Add(const_data_ptr_t data, idx_t len) {
		MD5Update(data, len);
	}
	void Add(const char *data);
	void Add(string_t string) {
		MD5Update(const_data_ptr_cast(string.GetData()), string.GetSize());
	}
	void Add(const string &data) {
		MD5Update(const_data_ptr_cast(data.c_str()), data.size());
	}

	//! Write the 16-byte (binary) digest to the specified location
	void Finish(data_ptr_t out_digest);
	//! Write the 32-character digest (in hexadecimal format) to the specified location
	void FinishHex(char *out_digest);
	//! Returns the 32-character digest (in hexadecimal format) as a string
	string FinishHex();

private:
	void MD5Update(const_data_ptr_t data, idx_t len);

	uint32_t buf[4];
	uint32_t bits[2];
	unsigned char in[64];
};

} // namespace goose
