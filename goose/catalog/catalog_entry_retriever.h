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

#include <functional>
#include <goose/common/enums/catalog_type.h>
#include <goose/common/enums/on_entry_not_found.h>
#include <goose/common/string.h>
#include <goose/parser/query_error_context.h>
#include <goose/catalog/catalog_entry/schema_catalog_entry.h>
#include <goose/catalog/catalog_search_path.h>
#include <goose/catalog/entry_lookup_info.h>

namespace goose {
    class ClientContext;
    class Catalog;
    class CatalogEntry;

    using catalog_entry_callback_t = std::function<void(CatalogEntry &)>;

    // Wraps the Catalog::GetEntry method
    class CatalogEntryRetriever {
    public:
        explicit CatalogEntryRetriever(ClientContext &context) : context(context) {
        }

        CatalogEntryRetriever(const CatalogEntryRetriever &other) : callback(other.callback), context(other.context) {
        }

    public:
        void Inherit(const CatalogEntryRetriever &parent);

        ClientContext &GetContext() {
            return context;
        }

        optional_ptr<CatalogEntry> GetEntry(const string &catalog, const string &schema,
                                            const EntryLookupInfo &lookup_info,
                                            OnEntryNotFound on_entry_not_found = OnEntryNotFound::THROW_EXCEPTION);

        optional_ptr<CatalogEntry> GetEntry(Catalog &catalog, const string &schema, const EntryLookupInfo &lookup_info,
                                            OnEntryNotFound on_entry_not_found = OnEntryNotFound::THROW_EXCEPTION);

        LogicalType GetType(const string &catalog, const string &schema, const string &name,
                            OnEntryNotFound on_entry_not_found = OnEntryNotFound::RETURN_NULL);

        LogicalType GetType(Catalog &catalog, const string &schema, const string &name,
                            OnEntryNotFound on_entry_not_found = OnEntryNotFound::RETURN_NULL);

        optional_ptr<SchemaCatalogEntry> GetSchema(const string &catalog, const EntryLookupInfo &schema_lookup,
                                                   OnEntryNotFound on_entry_not_found =
                                                           OnEntryNotFound::THROW_EXCEPTION);

        const CatalogSearchPath &GetSearchPath() const;

        void SetSearchPath(vector<CatalogSearchEntry> entries);

        void SetCallback(catalog_entry_callback_t callback);

        catalog_entry_callback_t GetCallback();

        optional_ptr<BoundAtClause> GetAtClause() const;

        void SetAtClause(optional_ptr<BoundAtClause> at_clause);

    private:
        optional_ptr<CatalogEntry> ReturnAndCallback(optional_ptr<CatalogEntry> result);

    private:
        //! (optional) callback, called on every successful entry retrieval
        catalog_entry_callback_t callback = nullptr;
        ClientContext &context;
        shared_ptr<CatalogSearchPath> search_path;
        optional_ptr<BoundAtClause> at_clause;
    };
} // namespace goose
