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

#include <goose/catalog/catalog_entry.h>
#include <goose/common/error_data.h>
#include <goose/parser/query_error_context.h>

namespace goose {
class BoundAtClause;

struct EntryLookupInfo {
public:
	EntryLookupInfo(CatalogType catalog_type, const string &name,
	                QueryErrorContext error_context = QueryErrorContext());
	EntryLookupInfo(CatalogType catalog_type, const string &name, optional_ptr<BoundAtClause> at_clause,
	                QueryErrorContext error_context);
	EntryLookupInfo(const EntryLookupInfo &parent, const string &name);
	EntryLookupInfo(const EntryLookupInfo &parent, optional_ptr<BoundAtClause> at_clause);

public:
	CatalogType GetCatalogType() const;
	const string &GetEntryName() const;
	const QueryErrorContext &GetErrorContext() const;
	const optional_ptr<BoundAtClause> GetAtClause() const;

	static EntryLookupInfo SchemaLookup(const EntryLookupInfo &parent, const string &schema_name);

private:
	CatalogType catalog_type;
	const string &name;
	optional_ptr<BoundAtClause> at_clause;
	QueryErrorContext error_context;
};

//! Return value of Catalog::LookupEntry
struct CatalogEntryLookup {
	optional_ptr<SchemaCatalogEntry> schema;
	optional_ptr<CatalogEntry> entry;
	ErrorData error;

	GOOSE_API bool Found() const {
		return entry;
	}
};

} // namespace goose
