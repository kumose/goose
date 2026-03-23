#include <goose/transaction/transaction_context.h>

#include <goose/common/exception.h>
#include <goose/common/exception/transaction_exception.h>
#include <goose/main/client_context.h>
#include <goose/main/client_data.h>
#include <goose/main/database.h>
#include <goose/transaction/meta_transaction.h>
#include <goose/main/attached_database.h>

namespace goose {

TransactionContext::TransactionContext(ClientContext &context)
    : context(context), auto_commit(true), current_transaction(nullptr) {
}

TransactionContext::~TransactionContext() {
	if (current_transaction) {
		try {
			Rollback(nullptr);
		} catch (std::exception &ex) {
			ErrorData data(ex);
			try {
				GOOSE_LOG_ERROR(context, "TransactionContext::~TransactionContext()\t\t" + data.Message());
			} catch (...) { // NOLINT
			}
		} catch (...) { // NOLINT
		}
	}
}

void TransactionContext::BeginTransaction() {
	if (current_transaction) {
		throw TransactionException("cannot start a transaction within a transaction");
	}
	auto start_timestamp = Timestamp::GetCurrentTimestamp();
	auto global_transaction_id = context.db->GetDatabaseManager().GetNewTransactionNumber();
	current_transaction = make_uniq<MetaTransaction>(context, start_timestamp, global_transaction_id);

	// Notify any registered state of transaction begin
	for (auto &state : context.registered_state->States()) {
		state->TransactionBegin(*current_transaction, context);
	}
}

void TransactionContext::Commit() {
	if (!current_transaction) {
		throw TransactionException("failed to commit: no transaction active");
	}
	auto transaction = std::move(current_transaction);
	ClearTransaction();
	auto error = transaction->Commit();
	// Notify any registered state of transaction commit
	if (error.HasError()) {
		for (auto const &s : context.registered_state->States()) {
			s->TransactionRollback(*transaction, context, error);
		}
		throw TransactionException("Failed to commit: %s", error.RawMessage());
	}
	for (auto &state : context.registered_state->States()) {
		state->TransactionCommit(*transaction, context);
	}
	transaction->Finalize();
}

void TransactionContext::SetAutoCommit(bool value) {
	auto_commit = value;
	if (!auto_commit && !current_transaction) {
		BeginTransaction();
	}
}

void TransactionContext::SetReadOnly() {
	current_transaction->SetReadOnly();
}

void TransactionContext::Rollback(optional_ptr<ErrorData> error) {
	if (!current_transaction) {
		throw TransactionException("failed to rollback: no transaction active");
	}
	auto transaction = std::move(current_transaction);
	ClearTransaction();
	context.client_data->profiler->Reset();

	ErrorData rollback_error;
	try {
		transaction->Rollback();
	} catch (std::exception &ex) {
		rollback_error = ErrorData(ex);
	}
	// Notify any registered state of transaction rollback
	for (auto const &s : context.registered_state->States()) {
		s->TransactionRollback(*transaction, context, error);
	}
	if (rollback_error.HasError()) {
		rollback_error.Throw();
	}
	transaction->Finalize();
}

void TransactionContext::ClearTransaction() {
	SetAutoCommit(true);
	current_transaction = nullptr;
}

idx_t TransactionContext::GetActiveQuery() {
	if (!current_transaction) {
		throw InternalException("GetActiveQuery called without active transaction");
	}
	return current_transaction->GetActiveQuery();
}

void TransactionContext::ResetActiveQuery() {
	if (current_transaction) {
		SetActiveQuery(MAXIMUM_QUERY_ID);
	}
}

void TransactionContext::SetActiveQuery(transaction_t query_number) {
	if (!current_transaction) {
		throw InternalException("SetActiveQuery called without active transaction");
	}
	current_transaction->SetActiveQuery(query_number);
}

} // namespace goose
