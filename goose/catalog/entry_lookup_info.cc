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
//


#include <goose/catalog/entry_lookup_info.h>

namespace goose {
    EntryLookupInfo::EntryLookupInfo(CatalogType catalog_type_p, const string &name_p,
                                     QueryErrorContext error_context_p)
        : catalog_type(catalog_type_p), name(name_p), error_context(error_context_p) {
    }

    EntryLookupInfo::EntryLookupInfo(CatalogType catalog_type_p, const string &name_p,
                                     optional_ptr<BoundAtClause> at_clause_p, QueryErrorContext error_context_p)
        : catalog_type(catalog_type_p), name(name_p), at_clause(at_clause_p), error_context(error_context_p) {
    }

    EntryLookupInfo::EntryLookupInfo(const EntryLookupInfo &parent, const string &name_p)
        : catalog_type(parent.catalog_type), name(name_p), at_clause(parent.at_clause),
          error_context(parent.error_context) {
    }

    EntryLookupInfo::EntryLookupInfo(const EntryLookupInfo &parent, optional_ptr<BoundAtClause> at_clause)
        : EntryLookupInfo(parent.catalog_type, parent.name, parent.at_clause ? parent.at_clause : at_clause,
                          parent.error_context) {
    }

    EntryLookupInfo EntryLookupInfo::SchemaLookup(const EntryLookupInfo &parent, const string &schema_name) {
        return EntryLookupInfo(CatalogType::SCHEMA_ENTRY, schema_name, parent.at_clause, parent.error_context);
    }

    CatalogType EntryLookupInfo::GetCatalogType() const {
        return catalog_type;
    }

    const string &EntryLookupInfo::GetEntryName() const {
        return name;
    }

    const QueryErrorContext &EntryLookupInfo::GetErrorContext() const {
        return error_context;
    }

    const optional_ptr<BoundAtClause> EntryLookupInfo::GetAtClause() const {
        return at_clause;
    }
} // namespace goose
