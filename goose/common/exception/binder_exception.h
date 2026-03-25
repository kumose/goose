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
