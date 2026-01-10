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
#include <goose/common/types/value.h>
#include <goose/parser/parsed_expression.h>

namespace goose {

enum class PragmaType : uint8_t { PRAGMA_STATEMENT, PRAGMA_CALL };

struct PragmaInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::PRAGMA_INFO;

public:
	PragmaInfo() : ParseInfo(TYPE) {
	}

	//! Name of the PRAGMA statement
	string name;
	//! Parameter list (if any)
	vector<unique_ptr<ParsedExpression>> parameters;
	//! Named parameter list (if any)
	case_insensitive_map_t<unique_ptr<ParsedExpression>> named_parameters;

public:
	unique_ptr<PragmaInfo> Copy() const;
	string ToString() const;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);
};

} // namespace goose
