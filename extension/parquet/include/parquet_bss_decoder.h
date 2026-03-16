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
#include "parquet_types.h"
#include "resizable_buffer.h"

namespace goose {

/// Decoder for the Byte Stream Split encoding
class BssDecoder {
public:
	/// Create a decoder object. buffer/buffer_len is the encoded data.
	BssDecoder(data_ptr_t buffer, uint32_t buffer_len) : buffer_(buffer, buffer_len), value_offset_(0) {
	}

public:
	template <typename T>
	void GetBatch(data_ptr_t values_target_ptr, uint32_t batch_size) {
		if (buffer_.len % sizeof(T) != 0) {
			goose::stringstream error;
			error << "Data buffer size for the BYTE_STREAM_SPLIT encoding (" << buffer_.len
			      << ") should be a multiple of the type size (" << sizeof(T) << ")";
			throw std::runtime_error(error.str());
		}
		uint32_t num_buffer_values = buffer_.len / sizeof(T);

		buffer_.available((value_offset_ + batch_size) * sizeof(T));

		for (uint32_t byte_offset = 0; byte_offset < sizeof(T); ++byte_offset) {
			data_ptr_t input_bytes = buffer_.ptr + byte_offset * num_buffer_values + value_offset_;
			for (uint32_t i = 0; i < batch_size; ++i) {
				values_target_ptr[byte_offset + i * sizeof(T)] = *(input_bytes + i);
			}
		}
		value_offset_ += batch_size;
	}

	template <typename T>
	void Skip(uint32_t batch_size) {
		if (buffer_.len % sizeof(T) != 0) {
			goose::stringstream error;
			error << "Data buffer size for the BYTE_STREAM_SPLIT encoding (" << buffer_.len
			      << ") should be a multiple of the type size (" << sizeof(T) << ")";
			throw std::runtime_error(error.str());
		}
		buffer_.available((value_offset_ + batch_size) * sizeof(T));
		value_offset_ += batch_size;
	}

private:
	ByteBuffer buffer_;
	uint32_t value_offset_;
};

} // namespace goose
