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

#include <goose/common/common.h>
#include <goose/common/enums/catalog_type.h>

namespace goose {

enum class CatalogType : uint8_t;

enum class ParseInfoType : uint8_t {
	ALTER_INFO,
	ATTACH_INFO,
	COPY_INFO,
	CREATE_INFO,
	CREATE_SECRET_INFO,
	DETACH_INFO,
	DROP_INFO,
	BOUND_EXPORT_DATA,
	LOAD_INFO,
	PRAGMA_INFO,
	SHOW_SELECT_INFO,
	TRANSACTION_INFO,
	VACUUM_INFO,
	COMMENT_ON_INFO,
	COMMENT_ON_COLUMN_INFO,
	COPY_DATABASE_INFO,
	UPDATE_EXTENSIONS_INFO
};

struct ParseInfo {
	explicit ParseInfo(ParseInfoType info_type) : info_type(info_type) {
	}
	virtual ~ParseInfo() {
	}

	ParseInfoType info_type;

public:
	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}

	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}

	virtual void Serialize(Serializer &serializer) const;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);
	static string QualifierToString(const string &catalog, const string &schema, const string &name);
	static string TypeToString(CatalogType type);
};

} // namespace goose
