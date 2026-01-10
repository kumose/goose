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
#include <goose/common/enum_util.h>
#include <goose/common/enums/on_create_conflict.h>
#include <goose/common/types/value.h>
#include <goose/catalog/dependency_list.h>

namespace goose {
struct AlterInfo;

struct CreateInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::CREATE_INFO;

public:
	explicit CreateInfo(CatalogType type, string schema = DEFAULT_SCHEMA, string catalog_p = INVALID_CATALOG)
	    : ParseInfo(TYPE), type(type), catalog(std::move(catalog_p)), schema(std::move(schema)),
	      on_conflict(OnCreateConflict::ERROR_ON_CONFLICT), temporary(false), internal(false) {
	}
	~CreateInfo() override {
	}

	//! The to-be-created catalog type
	CatalogType type;
	//! The catalog name of the entry
	string catalog;
	//! The schema name of the entry
	string schema;
	//! What to do on create conflict
	OnCreateConflict on_conflict;
	//! Whether or not the entry is temporary
	bool temporary;
	//! Whether or not the entry is an internal entry
	bool internal;
	//! The SQL string of the CREATE statement
	string sql;
	//! The inherent dependencies of the created entry
	LogicalDependencyList dependencies;
	//! User provided comment
	Value comment;
	//! Key-value tags with additional metadata
	InsertionOrderPreservingMap<string> tags;

public:
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<CreateInfo> Deserialize(Deserializer &deserializer);

	virtual unique_ptr<CreateInfo> Copy() const = 0;

	GOOSE_API void CopyProperties(CreateInfo &other) const;
	//! Generates an alter statement from the create statement - used for OnCreateConflict::ALTER_ON_CONFLICT
	GOOSE_API virtual unique_ptr<AlterInfo> GetAlterInfo() const;
	//! Returns a string like "CREATE (OR REPLACE) (TEMPORARY) <entry> (IF NOT EXISTS) " for TABLE/VIEW/TYPE/MACRO
	GOOSE_API string GetCreatePrefix(const string &entry) const;

	virtual string ToString() const {
		throw NotImplementedException("ToString not supported for this type of CreateInfo: '%s'",
		                              EnumUtil::ToString(info_type));
	}
};

} // namespace goose
