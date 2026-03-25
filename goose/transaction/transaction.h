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

#include <goose/catalog/catalog_entry/sequence_catalog_entry.h>
#include <goose/common/types/data_chunk.h>
#include <goose/transaction/transaction_data.h>
#include <goose/common/shared_ptr.h>
#include <goose/common/types-import.h>

namespace goose {
    class Catalog;
    class SequenceCatalogEntry;
    class SchemaCatalogEntry;

    class AttachedDatabase;
    class ColumnData;
    class ClientContext;
    class CatalogEntry;
    class DataTable;
    class DatabaseInstance;
    class LocalStorage;
    class MetaTransaction;
    class TransactionManager;
    class WriteAheadLog;

    class ChunkVectorInfo;

    struct DeleteInfo;
    struct UpdateInfo;
    struct DatabaseModificationType;

    //! The transaction object holds information about a currently running or past
    //! transaction
    class Transaction {
    public:
        GOOSE_API Transaction(TransactionManager &manager, ClientContext &context);

        GOOSE_API virtual ~Transaction();

        TransactionManager &manager;
        weak_ptr<ClientContext> context;
        //! The current active query for the transaction. Set to MAXIMUM_QUERY_ID if
        //! no query is active.
        atomic<transaction_t> active_query;

    public:
        GOOSE_API static Transaction &Get(ClientContext &context, AttachedDatabase &db);

        GOOSE_API static Transaction &Get(ClientContext &context, Catalog &catalog);

        //! Returns the transaction for the given context if it has already been started
        GOOSE_API static optional_ptr<Transaction> TryGet(ClientContext &context, AttachedDatabase &db);

        //! Whether or not the transaction has made any modifications to the database so far
        GOOSE_API bool IsReadOnly();

        //! Promotes the transaction to a read-write transaction
        GOOSE_API virtual void SetReadWrite();

        //! Sets the database modifications that are planned to be performed in this transaction
        GOOSE_API virtual void SetModifications(DatabaseModificationType type);

        virtual bool IsGooseTransaction() const {
            return false;
        }

    public:
        template<class TARGET>
        TARGET &Cast() {
            DynamicCastCheck<TARGET>(this);
            return reinterpret_cast<TARGET &>(*this);
        }

        template<class TARGET>
        const TARGET &Cast() const {
            DynamicCastCheck<TARGET>(this);
            return reinterpret_cast<const TARGET &>(*this);
        }

    private:
        bool is_read_only;
    };
} // namespace goose
