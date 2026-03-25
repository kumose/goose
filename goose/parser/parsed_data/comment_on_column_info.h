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
