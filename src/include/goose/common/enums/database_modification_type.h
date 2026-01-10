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

#include <goose/common/types.h>

namespace goose {

struct DatabaseModificationType {
public:
	static constexpr idx_t INSERT_DATA = 1ULL << 0ULL;
	static constexpr idx_t DELETE_DATA = 1ULL << 1ULL;
	static constexpr idx_t UPDATE_DATA = 1ULL << 2ULL;
	static constexpr idx_t ALTER_TABLE = 1ULL << 3ULL;
	static constexpr idx_t CREATE_CATALOG_ENTRY = 1ULL << 4ULL;
	static constexpr idx_t DROP_CATALOG_ENTRY = 1ULL << 5ULL;
	static constexpr idx_t SEQUENCE = 1ULL << 6ULL;
	static constexpr idx_t CREATE_INDEX = 1ULL << 7ULL;

	constexpr DatabaseModificationType() : value(0) {
	}
	constexpr DatabaseModificationType(idx_t value) : value(value) { // NOLINT : allow implicit conversion
	}

	inline constexpr DatabaseModificationType operator|(DatabaseModificationType b) const {
		return DatabaseModificationType(value | b.value);
	}
	inline DatabaseModificationType &operator|=(DatabaseModificationType b) {
		value |= b.value;
		return *this;
	}

	bool InsertData() const {
		return value & INSERT_DATA;
	}
	bool DeleteData() const {
		return value & DELETE_DATA;
	}
	bool UpdateData() const {
		return value & UPDATE_DATA;
	}
	bool AlterTable() const {
		return value & ALTER_TABLE;
	}
	bool CreateCatalogEntry() const {
		return value & CREATE_CATALOG_ENTRY;
	}
	bool DropCatalogEntry() const {
		return value & DROP_CATALOG_ENTRY;
	}
	bool Sequence() const {
		return value & SEQUENCE;
	}
	bool CreateIndex() const {
		return value & CREATE_INDEX;
	}

private:
	idx_t value;
};

} // namespace goose
