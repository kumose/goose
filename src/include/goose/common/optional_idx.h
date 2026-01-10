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

#include <goose/common/exception.h>

namespace goose {

class optional_idx {
	static constexpr const idx_t INVALID_INDEX = idx_t(-1);

public:
	optional_idx() : index(INVALID_INDEX) {
	}
	optional_idx(idx_t index) : index(index) { // NOLINT: allow implicit conversion from idx_t
		if (index == INVALID_INDEX) {
			throw InternalException("optional_idx cannot be initialized with an invalid index");
		}
	}

	static optional_idx Invalid() {
		return optional_idx();
	}

	bool IsValid() const {
		return index != INVALID_INDEX;
	}

	void SetInvalid() {
		index = INVALID_INDEX;
	}

	idx_t GetIndex() const {
		if (index == INVALID_INDEX) {
			throw InternalException("Attempting to get the index of an optional_idx that is not set");
		}
		return index;
	}

	inline bool operator==(const optional_idx &rhs) const {
		return index == rhs.index;
	}

	inline bool operator!=(const optional_idx &rhs) const {
		return index != rhs.index;
	}

private:
	idx_t index;
};

} // namespace goose
