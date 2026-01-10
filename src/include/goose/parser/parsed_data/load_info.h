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

namespace goose {

enum class LoadType : uint8_t { LOAD, INSTALL, FORCE_INSTALL };

struct LoadInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::LOAD_INFO;

public:
	LoadInfo() : ParseInfo(TYPE) {
	}

	string filename;
	string repository;
	bool repo_is_alias;
	string version;
	LoadType load_type;

public:
	unique_ptr<LoadInfo> Copy() const;
	string ToString() const;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);
};

} // namespace goose
