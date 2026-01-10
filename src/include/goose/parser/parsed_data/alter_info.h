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

#include <goose/common/enums/catalog_type.h>
#include <goose/parser/parsed_data/parse_info.h>
#include <goose/common/enums/on_entry_not_found.h>

namespace goose {

enum class AlterType : uint8_t {
	INVALID = 0,
	ALTER_TABLE = 1,
	ALTER_VIEW = 2,
	ALTER_SEQUENCE = 3,
	CHANGE_OWNERSHIP = 4,
	ALTER_SCALAR_FUNCTION = 5,
	ALTER_TABLE_FUNCTION = 6,
	SET_COMMENT = 7,
	SET_COLUMN_COMMENT = 8,
	ALTER_DATABASE = 9
};

struct AlterEntryData {
	AlterEntryData() {
	}
	AlterEntryData(string catalog_p, string schema_p, string name_p, OnEntryNotFound if_not_found)
	    : catalog(std::move(catalog_p)), schema(std::move(schema_p)), name(std::move(name_p)),
	      if_not_found(if_not_found) {
	}

	string catalog;
	string schema;
	string name;
	OnEntryNotFound if_not_found;
};

struct AlterInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::ALTER_INFO;

public:
	AlterInfo(AlterType type, string catalog, string schema, string name, OnEntryNotFound if_not_found);
	~AlterInfo() override;

	AlterType type;
	//! if exists
	OnEntryNotFound if_not_found;
	//! Catalog name to alter
	string catalog;
	//! Schema name to alter
	string schema;
	//! Entry name to alter
	string name;
	//! Allow altering internal entries
	bool allow_internal;

public:
	virtual CatalogType GetCatalogType() const = 0;
	virtual unique_ptr<AlterInfo> Copy() const = 0;
	virtual string ToString() const = 0;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);

	virtual string GetColumnName() const {
		return "";
	};

	AlterEntryData GetAlterEntryData() const;
	bool IsAddPrimaryKey() const;

protected:
	explicit AlterInfo(AlterType type);
};

} // namespace goose
