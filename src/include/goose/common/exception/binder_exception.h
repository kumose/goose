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
#include <goose/common/vector.h>
#include <goose/parser/query_error_context.h>

namespace goose {

class BinderException : public Exception {
public:
	GOOSE_API explicit BinderException(const string &msg);

	GOOSE_API explicit BinderException(const unordered_map<string, string> &extra_info, const string &msg);

	template <typename... ARGS>
	explicit BinderException(const string &msg, ARGS &&...params)
	    : BinderException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	template <typename... ARGS>
	explicit BinderException(const TableRef &ref, const string &msg, ARGS &&...params)
	    : BinderException(Exception::InitializeExtraInfo(ref), ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
	template <typename... ARGS>
	explicit BinderException(const ParsedExpression &expr, const string &msg, ARGS &&...params)
	    : BinderException(Exception::InitializeExtraInfo(expr), ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	template <typename... ARGS>
	explicit BinderException(const Expression &expr, const string &msg, ARGS &&...params)
	    : BinderException(Exception::InitializeExtraInfo(expr), ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	template <typename... ARGS>
	explicit BinderException(QueryErrorContext error_context, const string &msg, ARGS &&...params)
	    : BinderException(Exception::InitializeExtraInfo(error_context),
	                      ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	template <typename... ARGS>
	explicit BinderException(optional_idx error_location, const string &msg, ARGS &&...params)
	    : BinderException(Exception::InitializeExtraInfo(error_location),
	                      ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	static BinderException ColumnNotFound(const string &name, const vector<string> &similar_bindings,
	                                      QueryErrorContext context = QueryErrorContext());
	static BinderException NoMatchingFunction(const string &catalog_name, const string &schema_name, const string &name,
	                                          const vector<LogicalType> &arguments, const vector<string> &candidates);
	static BinderException Unsupported(ParsedExpression &expr, const string &message);
};

} // namespace goose
