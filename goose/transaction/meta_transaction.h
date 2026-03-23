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

#include <goose/common/common.h>
#include <goose/main/valid_checker.h>
#include <goose/common/types/timestamp.h>
#include <goose/common/optional_ptr.h>
#include <goose/common/reference_map.h>
#include <goose/common/error_data.h>
#include <goose/common/case_insensitive_map.h>
#include <goose/main/attached_database.h>

namespace goose {
class AttachedDatabase;
class ClientContext;
struct DatabaseModificationType;
class Transaction;

enum class TransactionState { UNCOMMITTED, COMMITTED, ROLLED_BACK };

struct TransactionReference {
	explicit TransactionReference(Transaction &transaction_p)
	    : state(TransactionState::UNCOMMITTED), transaction(transaction_p) {
	}

	TransactionState state;
	Transaction &transaction;
};

//! The MetaTransaction manages multiple transactions for different attached databases
class MetaTransaction {
public:
	GOOSE_API MetaTransaction(ClientContext &context, timestamp_t start_timestamp,
	                           transaction_t global_transaction_id);

	ClientContext &context;
	//! The timestamp when the transaction started
	timestamp_t start_timestamp;
	//! The global identifier of the transaction
	transaction_t global_transaction_id;
	//! The validity checker of the transaction
	ValidChecker transaction_validity;
	//! The active query number
	atomic<transaction_t> active_query;

public:
	GOOSE_API static MetaTransaction &Get(ClientContext &context);
	timestamp_t GetCurrentTransactionStartTimestamp() const {
		return start_timestamp;
	}

	Transaction &GetTransaction(AttachedDatabase &db);
	optional_ptr<Transaction> TryGetTransaction(AttachedDatabase &db);
	void RemoveTransaction(AttachedDatabase &db);

	ErrorData Commit();
	void Rollback();
	// Finalize the transaction after a COMMIT of ROLLBACK.
	void Finalize();

	idx_t GetActiveQuery();
	void SetActiveQuery(transaction_t query_number);

	void SetReadOnly();
	bool IsReadOnly() const;
	void ModifyDatabase(AttachedDatabase &db, DatabaseModificationType modification);
	optional_ptr<AttachedDatabase> ModifiedDatabase() {
		return modified_database;
	}
	const vector<reference<AttachedDatabase>> &OpenedTransactions() const {
		return all_transactions;
	}
	optional_ptr<AttachedDatabase> GetReferencedDatabase(const string &name);
	shared_ptr<AttachedDatabase> GetReferencedDatabaseOwning(const string &name);
	AttachedDatabase &UseDatabase(shared_ptr<AttachedDatabase> &database);
	void DetachDatabase(AttachedDatabase &database);

private:
	//! Lock to prevent all_transactions and transactions from getting out of sync.
	mutex lock;
	//! The set of active transactions for each database.
	reference_map_t<AttachedDatabase, TransactionReference> transactions;
	//! The set of referenced databases in invocation order.
	vector<reference<AttachedDatabase>> all_transactions;
	//! The database we are modifying. We can only modify one database per meta transaction.
	optional_ptr<AttachedDatabase> modified_database;
	//! Whether the meta transaction is marked as read only.
	bool is_read_only;
	//! Lock for referenced_databases.
	mutex referenced_database_lock;
	//! The set of used (referenced) databases.
	reference_map_t<AttachedDatabase, shared_ptr<AttachedDatabase>> referenced_databases;
	//! Map of name -> database for databases that are in-use by this transaction.
	case_insensitive_map_t<reference<AttachedDatabase>> used_databases;
};

} // namespace goose
