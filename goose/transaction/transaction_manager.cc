#include <goose/transaction/transaction_manager.h>

namespace goose {

TransactionManager::TransactionManager(AttachedDatabase &db) : db(db) {
}

TransactionManager::~TransactionManager() {
}

} // namespace goose
