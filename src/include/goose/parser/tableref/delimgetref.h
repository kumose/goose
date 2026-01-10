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

#include <goose/parser/tableref.h>

namespace goose {

class DelimGetRef : public TableRef {
public:
	explicit DelimGetRef(const vector<LogicalType> &types_p) : TableRef(TableReferenceType::DELIM_GET), types(types_p) {
		for (idx_t i = 0; i < types.size(); i++) {
			string column_name = "__internal_delim_get_" + std::to_string(i);
			internal_aliases.emplace_back(column_name);
		}
	}

	vector<string> internal_aliases;
	vector<LogicalType> types;

public:
	string ToString() const override;
	bool Equals(const TableRef &other_p) const override;

	unique_ptr<TableRef> Copy() override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<TableRef> Deserialize(Deserializer &source);
};
} // namespace goose
