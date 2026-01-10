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
#include <type_traits>

namespace goose {

struct EncodingUtil {
	// Encode unsigned integer, returns the number of bytes written
	template <class T>
	static idx_t EncodeUnsignedLEB128(data_ptr_t target, T value) {
		static_assert(std::is_integral<T>::value, "Must be integral");
		static_assert(std::is_unsigned<T>::value, "Must be unsigned");
		static_assert(sizeof(T) <= sizeof(uint64_t), "Must be uint64_t or smaller");

		idx_t offset = 0;
		do {
			uint8_t byte = value & 0x7F;
			value >>= 7;
			if (value != 0) {
				byte |= 0x80;
			}
			target[offset++] = byte;
		} while (value != 0);
		return offset;
	}

	// Decode unsigned integer, returns the number of bytes read
	template <class T>
	static idx_t DecodeUnsignedLEB128(const_data_ptr_t source, T &result) {
		static_assert(std::is_integral<T>::value, "Must be integral");
		static_assert(std::is_unsigned<T>::value, "Must be unsigned");
		static_assert(sizeof(T) <= sizeof(uint64_t), "Must be uint64_t or smaller");

		result = 0;
		idx_t shift = 0;
		idx_t offset = 0;
		uint8_t byte;
		do {
			byte = source[offset++];
			result |= static_cast<T>(byte & 0x7F) << shift;
			shift += 7;
		} while (byte & 0x80);

		return offset;
	}

	// Encode signed integer, returns the number of bytes written
	template <class T>
	static idx_t EncodeSignedLEB128(data_ptr_t target, T value) {
		static_assert(std::is_integral<T>::value, "Must be integral");
		static_assert(std::is_signed<T>::value, "Must be signed");
		static_assert(sizeof(T) <= sizeof(int64_t), "Must be int64_t or smaller");

		idx_t offset = 0;
		do {
			uint8_t byte = value & 0x7F;
			value >>= 7;

			// Determine whether more bytes are needed
			if ((value == 0 && (byte & 0x40) == 0) || (value == -1 && (byte & 0x40))) {
				target[offset++] = byte;
				break;
			} else {
				byte |= 0x80;
				target[offset++] = byte;
			}
		} while (true);
		return offset;
	}

	// Decode signed integer, returns the number of bytes read
	template <class T>
	static idx_t DecodeSignedLEB128(const_data_ptr_t source, T &result) {
		static_assert(std::is_integral<T>::value, "Must be integral");
		static_assert(std::is_signed<T>::value, "Must be signed");
		static_assert(sizeof(T) <= sizeof(int64_t), "Must be int64_t or smaller");

		// This is used to avoid undefined behavior when shifting into the sign bit
		using unsigned_type = typename std::make_unsigned<T>::type;

		result = 0;
		idx_t shift = 0;
		idx_t offset = 0;

		uint8_t byte;
		do {
			byte = source[offset++];
			result |= static_cast<unsigned_type>(byte & 0x7F) << shift;
			shift += 7;
		} while (byte & 0x80);

		// Sign-extend if the most significant bit of the last byte is set
		if (shift < sizeof(T) * 8 && (byte & 0x40)) {
			result |= -(static_cast<unsigned_type>(1) << shift);
		}
		return offset;
	}

	template <class T>
	static typename std::enable_if<std::is_signed<T>::value, idx_t>::type DecodeLEB128(const_data_ptr_t source,
	                                                                                   T &result) {
		return DecodeSignedLEB128(source, result);
	}

	template <class T>
	static typename std::enable_if<std::is_unsigned<T>::value, idx_t>::type DecodeLEB128(const_data_ptr_t source,
	                                                                                     T &result) {
		return DecodeUnsignedLEB128(source, result);
	}

	template <class T>
	static typename std::enable_if<std::is_signed<T>::value, idx_t>::type EncodeLEB128(data_ptr_t target, T value) {
		return EncodeSignedLEB128(target, value);
	}

	template <class T>
	static typename std::enable_if<std::is_unsigned<T>::value, idx_t>::type EncodeLEB128(data_ptr_t target, T value) {
		return EncodeUnsignedLEB128(target, value);
	}
};

} // namespace goose
