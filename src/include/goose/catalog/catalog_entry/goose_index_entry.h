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
