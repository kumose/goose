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


#include <goose/catalog/catalog_transaction.h>
#include <goose/catalog/catalog.h>
#include <goose/transaction/goose_transaction.h>
#include <goose/main/database.h>

namespace goose {
    CatalogTransaction::CatalogTransaction(Catalog &catalog, ClientContext &context) {
        auto &transaction = Transaction::Get(context, catalog);
        this->db = &DatabaseInstance::GetDatabase(context);
        if (!transaction.IsGooseTransaction()) {
            this->transaction_id = transaction_t(-1);
            this->start_time = transaction_t(-1);
        } else {
            auto &dtransaction = transaction.Cast<GooseTransaction>();
            this->transaction_id = dtransaction.transaction_id;
            this->start_time = dtransaction.start_time;
        }
        this->transaction = &transaction;
        this->context = &context;
    }

    CatalogTransaction::CatalogTransaction(DatabaseInstance &db, transaction_t transaction_id_p,
                                           transaction_t start_time_p)
        : db(&db), context(nullptr), transaction(nullptr), transaction_id(transaction_id_p), start_time(start_time_p) {
    }

    ClientContext &CatalogTransaction::GetContext() {
        if (!context) {
            throw InternalException("Attempting to get a context in a CatalogTransaction without a context");
        }
        return *context;
    }

    CatalogTransaction CatalogTransaction::GetSystemCatalogTransaction(ClientContext &context) {
        return CatalogTransaction(Catalog::GetSystemCatalog(context), context);
    }

    CatalogTransaction CatalogTransaction::GetSystemTransaction(DatabaseInstance &db) {
        return CatalogTransaction(db, 1, 1);
    }
} // namespace goose
