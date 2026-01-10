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
#include <goose/parser/simplified_token.h>

namespace goose {

class KeywordHelper {
public:
	//! Returns true if the given text matches a keyword of the parser
	static bool IsKeyword(const string &text);

	static KeywordCategory KeywordCategoryType(const string &text);

	static string EscapeQuotes(const string &text, char quote = '"');

	//! Returns true if the given string needs to be quoted when written as an identifier
	static bool RequiresQuotes(const string &text, bool allow_caps = true);

	//! Writes a string that is quoted
	static string WriteQuoted(const string &text, char quote = '\'');

	//! Writes a string that is optionally quoted + escaped so it can be used as an identifier
	static string WriteOptionallyQuoted(const string &text, char quote = '"', bool allow_caps = true);
};

} // namespace goose
