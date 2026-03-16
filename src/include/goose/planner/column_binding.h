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

#include <goose/common/common.h>
#include <goose/common/types-import.h>

#include <functional>

namespace goose {
class Serializer;
class Deserializer;

struct ColumnBinding {
	idx_t table_index;
	// This index is local to a Binding, and has no meaning outside of the context of the Binding that created it
	idx_t column_index;

	ColumnBinding() : table_index(DConstants::INVALID_INDEX), column_index(DConstants::INVALID_INDEX) {
	}
	ColumnBinding(idx_t table, idx_t column) : table_index(table), column_index(column) {
	}

	string ToString() const {
		return "#[" + to_string(table_index) + "." + to_string(column_index) + "]";
	}

	bool operator==(const ColumnBinding &rhs) const {
		return table_index == rhs.table_index && column_index == rhs.column_index;
	}

	bool operator!=(const ColumnBinding &rhs) const {
		return !(*this == rhs);
	}

	void Serialize(Serializer &serializer) const;
	static ColumnBinding Deserialize(Deserializer &deserializer);
};

} // namespace goose
