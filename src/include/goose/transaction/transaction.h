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
	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}

private:
	bool is_read_only;
};

} // namespace goose
