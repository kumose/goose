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
