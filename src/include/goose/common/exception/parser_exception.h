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
