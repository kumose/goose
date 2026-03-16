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

#include <goose/common/exception.h>
#include <goose/common/optional_idx.h>
#include <goose/common/types-import.h>

namespace goose {

class ParserException : public Exception {
public:
	GOOSE_API explicit ParserException(const string &msg);

	GOOSE_API explicit ParserException(const unordered_map<string, string> &extra_info, const string &msg);

	template <typename... ARGS>
	explicit ParserException(const string &msg, ARGS &&...params)
	    : ParserException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
	template <typename... ARGS>
	explicit ParserException(optional_idx error_location, const string &msg, ARGS &&...params)
	    : ParserException(Exception::InitializeExtraInfo(error_location),
	                      ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
	template <typename... ARGS>
	explicit ParserException(const ParsedExpression &expr, const string &msg, ARGS &&...params)
	    : ParserException(Exception::InitializeExtraInfo(expr), ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	static ParserException SyntaxError(const string &query, const string &error_message, optional_idx error_location);
};

} // namespace goose
