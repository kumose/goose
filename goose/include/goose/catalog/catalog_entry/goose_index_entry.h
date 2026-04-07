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

#include <goose/catalog/catalog_entry/index_catalog_entry.h>

namespace goose {
    class TableCatalogEntry;

    //! Wrapper class to allow copying a GooseIndexEntry (for altering the GooseIndexEntry metadata such as comments)
    struct IndexDataTableInfo {
        IndexDataTableInfo(shared_ptr<DataTableInfo> info_p, const string &index_name_p);

        //! Pointer to the DataTableInfo
        shared_ptr<DataTableInfo> info;
        //! The index to be removed on destruction
        string index_name;
    };

    //! A goose index entry
    class GooseIndexEntry : public IndexCatalogEntry {
    public:
        //! Create a GooseIndexEntry
        GooseIndexEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateIndexInfo &create_info,
                       TableCatalogEntry &table);

        GooseIndexEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateIndexInfo &create_info,
                       shared_ptr<IndexDataTableInfo> storage_info);

        unique_ptr<CatalogEntry> Copy(ClientContext &context) const override;

        void Rollback(CatalogEntry &prev_entry) override;

        //! The indexed table information
        shared_ptr<IndexDataTableInfo> info;
        //! We need the initial size of the index after the CREATE INDEX statement,
        //! as it is necessary to determine the auto checkpoint threshold
        idx_t initial_index_size;

    public:
        string GetSchemaName() const override;

        string GetTableName() const override;

        DataTableInfo &GetDataTableInfo() const;

        //! Drops in-memory index data and marks all blocks on disk as free blocks, allowing to reclaim them
        void CommitDrop();
    };
} // namespace goose
