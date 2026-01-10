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

#include <goose/common/enums/prepared_statement_mode.h>
#include <goose/common/exception/transaction_exception.h>
#include <goose/common/optional_ptr.h>
#include <goose/main/config.h>
#include <goose/main/valid_checker.h>
#include <goose/planner/expression/bound_parameter_data.h>
#include <goose/transaction/meta_transaction.h>
#include <goose/transaction/transaction_manager.h>
#include <goose/main/database_manager.h>
#include <goose/main/client_context.h>
#include <mutex>

namespace goose {
class ClientContext;
class ErrorData;
class MetaTransaction;
class PreparedStatementData;
class SQLStatement;
struct PendingQueryParameters;
class RegisteredStateManager;

enum class RebindQueryInfo { DO_NOT_REBIND, ATTEMPT_TO_REBIND };

struct PreparedStatementCallbackInfo {
	PreparedStatementCallbackInfo(PreparedStatementData &prepared_statement, const PendingQueryParameters &parameters)
	    : prepared_statement(prepared_statement), parameters(parameters) {
	}

	PreparedStatementData &prepared_statement;
	const PendingQueryParameters &parameters;
};

struct BindPreparedStatementCallbackInfo {
	PreparedStatementData &prepared_statement;
	optional_ptr<case_insensitive_map_t<BoundParameterData>> parameters;
};

//! ClientContextState is virtual base class for ClientContext-local (or Query-Local, using QueryEnd callback) state
//! e.g. caches that need to live as long as a ClientContext or Query.
class ClientContextState {
public:
	virtual ~ClientContextState() = default;
	virtual void QueryBegin(ClientContext &context) {
	}
	virtual void QueryEnd() {
	}
	virtual void QueryEnd(ClientContext &context) {
		QueryEnd();
	}
	virtual void QueryEnd(ClientContext &context, optional_ptr<ErrorData> error) {
		QueryEnd(context);
	}
	virtual void TransactionBegin(MetaTransaction &transaction, ClientContext &context) {
	}
	virtual void TransactionCommit(MetaTransaction &transaction, ClientContext &context) {
	}
	virtual void TransactionRollback(MetaTransaction &transaction, ClientContext &context) {
	}
	virtual void TransactionRollback(MetaTransaction &transaction, ClientContext &context,
	                                 optional_ptr<ErrorData> error) {
		TransactionRollback(transaction, context);
	}
	virtual bool CanRequestRebind() {
		return false;
	}
	virtual RebindQueryInfo OnPlanningError(ClientContext &context, SQLStatement &statement, ErrorData &error) {
		return RebindQueryInfo::DO_NOT_REBIND;
	}
	virtual RebindQueryInfo OnFinalizePrepare(ClientContext &context, PreparedStatementData &prepared_statement,
	                                          PreparedStatementMode mode) {
		return RebindQueryInfo::DO_NOT_REBIND;
	}
	virtual RebindQueryInfo OnExecutePrepared(ClientContext &context, PreparedStatementCallbackInfo &info,
	                                          RebindQueryInfo current_rebind) {
		return RebindQueryInfo::DO_NOT_REBIND;
	}
	virtual RebindQueryInfo OnRebindPreparedStatement(ClientContext &context, BindPreparedStatementCallbackInfo &info,
	                                                  RebindQueryInfo current_rebind) {
		return RebindQueryInfo::DO_NOT_REBIND;
	}
	virtual void WriteProfilingInformation(std::ostream &ss) {
	}
	virtual void OnTaskStart(ClientContext &context) {
	}
	virtual void OnTaskStop(ClientContext &context) {
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
};

class RegisteredStateManager {
public:
	template <class T, typename... ARGS>
	shared_ptr<T> GetOrCreate(const string &key, ARGS &&... args) {
		lock_guard<mutex> l(lock);
		auto lookup = registered_state.find(key);
		if (lookup != registered_state.end()) {
			return shared_ptr_cast<ClientContextState, T>(lookup->second);
		}
		auto cache = make_shared_ptr<T>(std::forward<ARGS>(args)...);
		registered_state[key] = cache;
		return cache;
	}

	template <class T>
	shared_ptr<T> Get(const string &key) {
		lock_guard<mutex> l(lock);
		auto lookup = registered_state.find(key);
		if (lookup == registered_state.end()) {
			return nullptr;
		}
		return shared_ptr_cast<ClientContextState, T>(lookup->second);
	}

	void Insert(const string &key, shared_ptr<ClientContextState> state_p) {
		lock_guard<mutex> l(lock);
		registered_state.insert(make_pair(key, std::move(state_p)));
	}

	void Remove(const string &key) {
		lock_guard<mutex> l(lock);
		registered_state.erase(key);
	}

	vector<shared_ptr<ClientContextState>> States() {
		lock_guard<mutex> l(lock);
		vector<shared_ptr<ClientContextState>> states;
		for (auto &entry : registered_state) {
			states.push_back(entry.second);
		}
		return states;
	}

private:
	mutex lock;
	unordered_map<string, shared_ptr<ClientContextState>> registered_state;
};

} // namespace goose
