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

#include <goose/parser/parsed_data/parse_info.h>
#include <goose/common/types/value.h>

namespace goose {
class TableCatalogEntry;

struct ExportedTableData {
	//! Name of the exported table
	string table_name;

	//! Name of the schema
	string schema_name;

	//! Name of the database
	string database_name;

	//! Path to be exported
	string file_path;
	//! Not Null columns, if any
	vector<string> not_null_columns;

	void Serialize(Serializer &serializer) const;
	static ExportedTableData Deserialize(Deserializer &deserializer);
};

struct ExportedTableInfo {
	ExportedTableInfo(TableCatalogEntry &entry, ExportedTableData table_data_p, vector<string> &not_null_columns_p);
	ExportedTableInfo(ClientContext &context, ExportedTableData table_data);

	TableCatalogEntry &entry;
	ExportedTableData table_data;

	void Serialize(Serializer &serializer) const;
	static ExportedTableInfo Deserialize(Deserializer &deserializer);

private:
	static TableCatalogEntry &GetEntry(ClientContext &context, const ExportedTableData &table_data);
};

struct BoundExportData : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::BOUND_EXPORT_DATA;

public:
	BoundExportData() : ParseInfo(TYPE) {
	}

	vector<ExportedTableInfo> data;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);
};

} // namespace goose
