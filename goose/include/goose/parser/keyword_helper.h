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
