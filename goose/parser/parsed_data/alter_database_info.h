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

#include <goose/parser/parsed_data/alter_info.h>

namespace goose {

enum class AlterDatabaseType : uint8_t { RENAME_DATABASE = 0 };

struct AlterDatabaseInfo : public AlterInfo {
public:
	explicit AlterDatabaseInfo(AlterDatabaseType alter_database_type);
	AlterDatabaseInfo(AlterDatabaseType alter_database_type, string catalog_p, OnEntryNotFound if_not_found);
	~AlterDatabaseInfo() override;

	AlterDatabaseType alter_database_type;

public:
	CatalogType GetCatalogType() const override;
	string ToString() const override = 0;

	static unique_ptr<AlterInfo> Deserialize(Deserializer &deserializer);

protected:
	void Serialize(Serializer &serializer) const override;
};

struct RenameDatabaseInfo : public AlterDatabaseInfo {
public:
	RenameDatabaseInfo();
	RenameDatabaseInfo(string catalog_p, string new_name_p, OnEntryNotFound if_not_found);

	string new_name;

public:
	unique_ptr<AlterInfo> Copy() const override;
	string ToString() const override;

	static unique_ptr<AlterDatabaseInfo> Deserialize(Deserializer &deserializer);

protected:
	void Serialize(Serializer &serializer) const override;
};

} // namespace goose
