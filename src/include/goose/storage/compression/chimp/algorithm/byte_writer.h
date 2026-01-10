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
