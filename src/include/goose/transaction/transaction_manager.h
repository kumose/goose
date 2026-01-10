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

#include <goose/catalog/catalog_set.h>
#include <goose/common/common.h>
#include <goose/common/types-import.h>
#include <goose/common/vector.h>
#include <goose/common/error_data.h>
#include <goose/common/types-import.h>

namespace goose {

class AttachedDatabase;
class ClientContext;
class Catalog;
struct ClientLockWrapper;
class DatabaseInstance;
class Transaction;

//! The Transaction Manager is responsible for creating and managing
//! transactions
class TransactionManager {
public:
	explicit TransactionManager(AttachedDatabase &db);
	virtual ~TransactionManager();

	//! Start a new transaction
	virtual Transaction &StartTransaction(ClientContext &context) = 0;
	//! Commit the given transaction. Returns a non-empty error message on failure.
	virtual ErrorData CommitTransaction(ClientContext &context, Transaction &transaction) = 0;
	//! Rollback the given transaction
	virtual void RollbackTransaction(Transaction &transaction) = 0;

	virtual void Checkpoint(ClientContext &context, bool force = false) = 0;

	static TransactionManager &Get(AttachedDatabase &db);

	virtual bool IsGooseTransactionManager() {
		return false;
	}

	AttachedDatabase &GetDB() {
		return db;
	}

protected:
	//! The attached database
	AttachedDatabase &db;

public:
	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		D_ASSERT(dynamic_cast<const TARGET *>(this));
		return reinterpret_cast<const TARGET &>(*this);
	}
};

} // namespace goose
