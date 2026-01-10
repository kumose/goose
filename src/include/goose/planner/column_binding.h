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
