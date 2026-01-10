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
