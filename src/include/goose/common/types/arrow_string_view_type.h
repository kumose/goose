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

#include <goose/common/numeric_utils.h>

namespace goose {

struct ArrowStringViewConstants {
public:
	static constexpr uint8_t MAX_INLINED_BYTES = 12 * sizeof(char);
	static constexpr uint8_t PREFIX_BYTES = 4 * sizeof(char);

public:
	ArrowStringViewConstants() = delete;
};

union arrow_string_view_t {
	arrow_string_view_t() {
	}

	//! Constructor for inlined arrow string views
	arrow_string_view_t(int32_t length, const char *data) {
		D_ASSERT(length <= ArrowStringViewConstants::MAX_INLINED_BYTES);
		inlined.length = length;
		memcpy(inlined.data, data, UnsafeNumericCast<idx_t>(length));
		if (length < ArrowStringViewConstants::MAX_INLINED_BYTES) {
			// have to 0 pad
			uint8_t remaining_bytes = ArrowStringViewConstants::MAX_INLINED_BYTES - NumericCast<uint8_t>(length);

			memset(&inlined.data[length], '\0', remaining_bytes);
		}
	}

	//! Constructor for non-inlined arrow string views
	arrow_string_view_t(int32_t length, const char *data, int32_t buffer_idx, int32_t offset) {
		D_ASSERT(length > ArrowStringViewConstants::MAX_INLINED_BYTES);
		ref.length = length;
		memcpy(ref.prefix, data, ArrowStringViewConstants::PREFIX_BYTES);
		ref.buffer_index = buffer_idx;
		ref.offset = offset;
	}

	//! Representation of inlined arrow string views
	struct {
		int32_t length;
		char data[ArrowStringViewConstants::MAX_INLINED_BYTES];
	} inlined;

	//! Representation of non-inlined arrow string views
	struct {
		int32_t length;
		char prefix[ArrowStringViewConstants::PREFIX_BYTES];
		int32_t buffer_index;
		int32_t offset;
	} ref;

	int32_t Length() const {
		return inlined.length;
	}
	bool IsInline() const {
		return Length() <= ArrowStringViewConstants::MAX_INLINED_BYTES;
	}

	const char *GetInlineData() const {
		return IsInline() ? inlined.data : ref.prefix;
	}
	int32_t GetBufferIndex() {
		D_ASSERT(!IsInline());
		return ref.buffer_index;
	}
	int32_t GetOffset() {
		D_ASSERT(!IsInline());
		return ref.offset;
	}
};

} // namespace goose
