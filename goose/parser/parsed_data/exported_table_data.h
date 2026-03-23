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
