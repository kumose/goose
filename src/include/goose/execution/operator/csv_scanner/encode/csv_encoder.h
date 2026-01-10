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
