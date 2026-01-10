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

namespace goose {

//! Simplified tokens are a simplified (dense) representation of the lexer
//! Used for simple syntax highlighting in the tests
enum class SimplifiedTokenType : uint8_t {
	SIMPLIFIED_TOKEN_IDENTIFIER,
	SIMPLIFIED_TOKEN_NUMERIC_CONSTANT,
	SIMPLIFIED_TOKEN_STRING_CONSTANT,
	SIMPLIFIED_TOKEN_OPERATOR,
	SIMPLIFIED_TOKEN_KEYWORD,
	SIMPLIFIED_TOKEN_COMMENT,
	SIMPLIFIED_TOKEN_ERROR,
	SIMPLIFIED_TOKEN_ERROR_EMPHASIS,
	SIMPLIFIED_TOKEN_ERROR_SUGGESTION
};

struct SimplifiedToken {
	SimplifiedTokenType type;
	idx_t start;
};

enum class KeywordCategory : uint8_t {
	KEYWORD_RESERVED,
	KEYWORD_UNRESERVED,
	KEYWORD_TYPE_FUNC,
	KEYWORD_COL_NAME,
	KEYWORD_NONE
};

struct ParserKeyword {
	string name;
	KeywordCategory category;
};

} // namespace goose
