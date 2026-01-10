#include <goose/transaction/transaction.h>
#include <goose/transaction/meta_transaction.h>
#include <goose/transaction/transaction_manager.h>
#include <goose/main/client_context.h>

namespace goose {

Transaction::Transaction(TransactionManager &manager_p, ClientContext &context_p)
    : manager(manager_p), context(context_p.shared_from_this()), active_query(MAXIMUM_QUERY_ID), is_read_only(true) {
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
