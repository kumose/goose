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

#include <goose/common/constants.h>

namespace goose {
class DataTable;
class RowVersionManager;

struct DeleteInfo {
	DataTable *table;
	RowVersionManager *version_info;
	idx_t vector_idx;
	idx_t count;
	idx_t base_row;
	//! Whether or not row ids are consecutive (0, 1, 2, ..., count).
	//! If this is true no rows are stored and `rows` should not be accessed.
	bool is_consecutive;

	uint16_t *GetRows() {
		if (is_consecutive) {
			throw InternalException("DeleteInfo is consecutive - rows are not accessible");
		}
		return rows;
	}
	const uint16_t *GetRows() const {
		if (is_consecutive) {
			throw InternalException("DeleteInfo is consecutive - rows are not accessible");
		}
		return rows;
	}

private:
	//! The per-vector row identifiers (actual row id is base_row + rows[x])
	uint16_t rows[1];
};

} // namespace goose
