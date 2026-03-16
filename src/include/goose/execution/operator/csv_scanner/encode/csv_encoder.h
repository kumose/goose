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

#include <goose/common/typedefs.h>
#include <goose/common/file_system.h>
#include <goose/function/encoding_function.h>
#include <goose/main/client_context.h>

namespace goose {

struct DBConfig;

//! Struct that holds encoder buffers
struct CSVEncoderBuffer {
	CSVEncoderBuffer() : encoded_buffer_size(0) {};
	void Initialize(idx_t encoded_buffer_size);

	char *Ptr() const;

	idx_t GetCapacity() const;

	idx_t GetSize() const;

	void SetSize(const idx_t buffer_size);

	bool HasDataToRead() const;

	void Reset();
	idx_t cur_pos = 0;
	//! The actual encoded buffer size, from the last file_handle read.
	idx_t actual_encoded_buffer_size = 0;
	//! If this is the last buffer
	bool last_buffer = false;

private:
	//! The encoded buffer, we only have one per file, so we cache it and make sure to pass over unused bytes.
	goose::unique_ptr<char[]> encoded_buffer;
	//! The encoded buffer size is defined as buffer_size/GetRatio()
	idx_t encoded_buffer_size;
};

class CSVEncoder {
public:
	//! Constructor, basically takes an encoding and the output buffer size
	CSVEncoder(ClientContext &context, const string &encoding_name, idx_t buffer_size);
	//! Main encode function, it reads the file into an encoded buffer and converts it to the output buffer
	idx_t Encode(FileHandle &file_handle_input, char *output_buffer, const idx_t decoded_buffer_size);
	string encoding_name;

private:
	QueryContext context;

	//! The actual encoded buffer
	CSVEncoderBuffer encoded_buffer;
	//! Potential remaining bytes
	CSVEncoderBuffer remaining_bytes_buffer;
	//! Actual Encoding Function
	optional_ptr<EncodingFunction> encoding_function;
	//! Pass-on Byte, used to check if we are done with the file, but must be appended to next buffer
	char pass_on_byte;
	bool has_pass_on_byte = false;
};
} // namespace goose
