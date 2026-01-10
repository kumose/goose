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
