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
#include <goose/common/types-import.h>

namespace goose {

struct TableColumn {
	TableColumn() = default;
	TableColumn(string name_p, LogicalType type_p) : name(std::move(name_p)), type(std::move(type_p)) {
	}

	string name;
	LogicalType type;

	void Serialize(Serializer &serializer) const;
	static TableColumn Deserialize(Deserializer &deserializer);
};

using virtual_column_map_t = unordered_map<column_t, TableColumn>;

} // namespace goose
