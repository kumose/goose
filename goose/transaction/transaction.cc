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

#include <goose/transaction/transaction.h>
#include <goose/transaction/meta_transaction.h>
#include <goose/transaction/transaction_manager.h>
#include <goose/main/client_context.h>

namespace goose {
    Transaction::Transaction(TransactionManager &manager_p, ClientContext &context_p)
        : manager(manager_p), context(context_p.shared_from_this()), active_query(MAXIMUM_QUERY_ID),
          is_read_only(true) {
    }

    Transaction::~Transaction() {
    }

    bool Transaction::IsReadOnly() {
        return is_read_only;
    }

    void Transaction::SetReadWrite() {
        D_ASSERT(is_read_only);
        is_read_only = false;
    }

    void Transaction::SetModifications(DatabaseModificationType type) {
    }
} // namespace goose
