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
