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
#include <goose/common/string.h>
#include <goose/common/enums/catalog_type.h>
#include <goose/parser/query_error_context.h>
#include <goose/common/types-import.h>

namespace goose {
struct EntryLookupInfo;

class CatalogException : public Exception {
public:
	GOOSE_API explicit CatalogException(const string &msg);

	GOOSE_API explicit CatalogException(const unordered_map<string, string> &extra_info, const string &msg);

	template <typename... ARGS>
	explicit CatalogException(const string &msg, ARGS &&...params)
	    : CatalogException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	template <typename... ARGS>
	explicit CatalogException(QueryErrorContext error_context, const string &msg, ARGS &&...params)
	    : CatalogException(Exception::InitializeExtraInfo(error_context),
	                       ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	static CatalogException MissingEntry(const EntryLookupInfo &lookup_info, const string &suggestion);
	static CatalogException MissingEntry(CatalogType type, const string &name, const string &suggestion,
	                                     QueryErrorContext context = QueryErrorContext());
	static CatalogException MissingEntry(const string &type, const string &name, const vector<string> &suggestions,
	                                     QueryErrorContext context = QueryErrorContext());
	static CatalogException EntryAlreadyExists(CatalogType type, const string &name,
	                                           QueryErrorContext context = QueryErrorContext());
};

} // namespace goose
