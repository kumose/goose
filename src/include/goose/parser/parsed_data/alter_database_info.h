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
