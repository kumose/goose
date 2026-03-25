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


#include <goose/catalog/catalog_entry/schema_catalog_entry.h>
#include <goose/catalog/default/default_schemas.h>

#include <goose/catalog/catalog.h>
#include <goose/common/types-import.h>
#include <goose/common/exception.h>
#include <goose/catalog/dependency_list.h>
#include <goose/parser/parsed_data/create_schema_info.h>
#include <goose/common/types-import.h>

namespace goose {
    SchemaCatalogEntry::SchemaCatalogEntry(Catalog &catalog, CreateSchemaInfo &info)
        : InCatalogEntry(CatalogType::SCHEMA_ENTRY, catalog, info.schema) {
        this->internal = info.internal;
        this->comment = info.comment;
        this->tags = info.tags;
    }

    CatalogTransaction SchemaCatalogEntry::GetCatalogTransaction(ClientContext &context) {
        return CatalogTransaction(catalog, context);
    }

    optional_ptr<CatalogEntry> SchemaCatalogEntry::CreateIndex(ClientContext &context, CreateIndexInfo &info,
                                                               TableCatalogEntry &table) {
        return CreateIndex(GetCatalogTransaction(context), info, table);
    }

    SimilarCatalogEntry SchemaCatalogEntry::GetSimilarEntry(CatalogTransaction transaction,
                                                            const EntryLookupInfo &lookup_info) {
        SimilarCatalogEntry result;
        Scan(transaction.GetContext(), lookup_info.GetCatalogType(), [&](CatalogEntry &entry) {
            auto entry_score = StringUtil::SimilarityRating(entry.name, lookup_info.GetEntryName());
            if (entry_score > result.score) {
                result.score = entry_score;
                result.name = entry.name;
            }
        });
        return result;
    }

    optional_ptr<CatalogEntry> SchemaCatalogEntry::GetEntry(CatalogTransaction transaction, CatalogType type,
                                                            const string &name) {
        EntryLookupInfo lookup_info(type, name);
        return LookupEntry(transaction, lookup_info);
    }

    //! This should not be used, it's only implemented to not put the burden of implementing it on every derived class of
    //! SchemaCatalogEntry
    CatalogSet::EntryLookup SchemaCatalogEntry::LookupEntryDetailed(CatalogTransaction transaction,
                                                                    const EntryLookupInfo &lookup_info) {
        CatalogSet::EntryLookup result;
        result.result = LookupEntry(transaction, lookup_info);
        if (!result.result) {
            result.reason = CatalogSet::EntryLookup::FailureReason::DELETED;
        } else {
            result.reason = CatalogSet::EntryLookup::FailureReason::SUCCESS;
        }
        return result;
    }

    unique_ptr<CreateInfo> SchemaCatalogEntry::GetInfo() const {
        auto result = make_uniq<CreateSchemaInfo>();
        result->schema = name;
        result->comment = comment;
        result->tags = tags;
        return result;
    }

    string SchemaCatalogEntry::ToSQL() const {
        auto create_schema_info = GetInfo();
        return create_schema_info->ToString();
    }
} // namespace goose
