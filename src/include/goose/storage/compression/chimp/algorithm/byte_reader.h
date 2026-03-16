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

#include <goose/goose-c.h>
#include <goose/common/fast_mem.h>
#include <goose/common/exception.h>

namespace goose {

class ByteReader {
public:
	ByteReader() : buffer(nullptr), index(0) {
	}

public:
	void SetStream(const uint8_t *buffer) {
		this->buffer = buffer;
		index = 0;
	}

	size_t Index() const {
		return index;
	}

	template <class T>
	T ReadValue() {
		auto result = Load<T>(buffer + index);
		index += sizeof(T);
		return result;
	}

	template <class T, uint8_t SIZE>
	T ReadValue() {
		return ReadValue<T>(SIZE);
	}

	template <class T>
	inline T ReadValue(uint8_t bytes, uint8_t trailing_zero) {
		T result = 0;
		switch (bytes) {
			// LCOV_EXCL_START
		case 1:
			result = Load<uint8_t>(buffer + index);
			index++;
			return result;
		case 2:
			result = Load<uint16_t>(buffer + index);
			index += 2;
			return result;
		case 3:
			memcpy(&result, (void *)(buffer + index), 3);
			index += 3;
			return result;
		case 4:
			result = Load<uint32_t>(buffer + index);
			index += 4;
			return result;
		case 5:
			memcpy(&result, (void *)(buffer + index), 5);
			index += 5;
			return result;
		case 6:
			memcpy(&result, (void *)(buffer + index), 6);
			index += 6;
			return result;
		case 7:
			memcpy(&result, (void *)(buffer + index), 7);
			index += 7;
			return result;
			// LCOV_EXCL_STOP
		default:
			if (trailing_zero < 8) {
				result = Load<T>(buffer + index);
				index += sizeof(T);
				return result;
			}
			return result;
		}
	}

private:
	const uint8_t *buffer;
	uint32_t index;
};

template <>
inline uint32_t ByteReader::ReadValue(uint8_t bytes, uint8_t trailing_zero) {
	uint32_t result = 0;
	switch (bytes) {
	case 0:
		// LCOV_EXCL_START
		if (trailing_zero < 8) {
			result = Load<uint32_t>(buffer + index);
			index += sizeof(uint32_t);
			return result;
		}
		return result;
	case 1:
		result = Load<uint8_t>(buffer + index);
		index++;
		return result;
	case 2:
		result = Load<uint16_t>(buffer + index);
		index += 2;
		return result;
	case 3:
		memcpy(&result, (void *)(buffer + index), 3);
		index += 3;
		return result;
	case 4:
		result = Load<uint32_t>(buffer + index);
		index += 4;
		return result;
		// LCOV_EXCL_STOP
	default:
		throw InternalException("Write of %llu bytes attempted into address pointing to 4 byte value", bytes);
	}
}
} // namespace goose
