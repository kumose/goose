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
#include <goose/common/helper.h>

namespace goose {

template <bool EMPTY>
class ByteWriter {
public:
	ByteWriter() : buffer(nullptr), index(0) {
	}

public:
	idx_t BytesWritten() const {
		return index;
	}

	void Flush() {
	}

	void ByteAlign() {
	}

	void SetStream(uint8_t *buffer) {
		this->buffer = buffer;
		this->index = 0;
	}

	template <class T, uint8_t SIZE>
	void WriteValue(const T &value) {
		const uint8_t bytes = (SIZE >> 3) + ((SIZE & 7) != 0);
		if (!EMPTY) {
			memcpy((void *)(buffer + index), &value, bytes);
		}
		index += bytes;
	}

	template <class T>
	void WriteValue(const T &value, const uint8_t &size) {
		const uint8_t bytes = (size >> 3) + ((size & 7) != 0);
		if (!EMPTY) {
			memcpy((void *)(buffer + index), &value, bytes);
		}
		index += bytes;
	}

private:
private:
	uint8_t *buffer;
	idx_t index;
};

} // namespace goose
