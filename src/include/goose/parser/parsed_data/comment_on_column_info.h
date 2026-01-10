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
#include <goose/common/types/value.h>
#include <goose/parser/parsed_data/alter_info.h>

namespace goose {
class CatalogEntryRetriever;
class ClientContext;
class CatalogEntry;

struct SetColumnCommentInfo : public AlterInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::COMMENT_ON_COLUMN_INFO;

public:
	SetColumnCommentInfo();
	SetColumnCommentInfo(string catalog, string schema, string name, string column_name, Value comment_value,
	                     OnEntryNotFound if_not_found);

	//! The resolved Catalog Type
	CatalogType catalog_entry_type;

	//! name of the column to comment on
	string column_name;
	//! The comment, can be NULL or a string
	Value comment_value;

public:
	optional_ptr<CatalogEntry> TryResolveCatalogEntry(CatalogEntryRetriever &retriever);
	unique_ptr<AlterInfo> Copy() const override;
	CatalogType GetCatalogType() const override;
	string ToString() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<AlterInfo> Deserialize(Deserializer &deserializer);
};

} // namespace goose
