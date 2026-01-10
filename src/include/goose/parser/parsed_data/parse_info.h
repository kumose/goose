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
