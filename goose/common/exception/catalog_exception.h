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
