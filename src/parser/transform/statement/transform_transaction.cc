#include <goose/parser/statement/transaction_statement.h>
#include <goose/parser/transformer.h>

namespace goose {

TransactionType TransformTransactionType(cantor::PGTransactionStmtKind kind) {
	switch (kind) {
	case cantor::PG_TRANS_STMT_BEGIN:
	case cantor::PG_TRANS_STMT_START:
		return TransactionType::BEGIN_TRANSACTION;
	case cantor::PG_TRANS_STMT_COMMIT:
		return TransactionType::COMMIT;
	case cantor::PG_TRANS_STMT_ROLLBACK:
		return TransactionType::ROLLBACK;
	default:
		throw NotImplementedException("Transaction type %d not implemented yet", kind);
	}
}

TransactionModifierType TransformTransactionModifier(cantor::PGTransactionStmtType type) {
	switch (type) {
	case cantor::PG_TRANS_TYPE_DEFAULT:
		return TransactionModifierType::TRANSACTION_DEFAULT_MODIFIER;
	case cantor::PG_TRANS_TYPE_READ_ONLY:
		return TransactionModifierType::TRANSACTION_READ_ONLY;
	case cantor::PG_TRANS_TYPE_READ_WRITE:
		return TransactionModifierType::TRANSACTION_READ_WRITE;
	default:
		throw NotImplementedException("Transaction modifier %d not implemented yet", type);
	}
}

unique_ptr<TransactionStatement> Transformer::TransformTransaction(cantor::PGTransactionStmt &stmt) {
	//	stmt.transaction_type
	auto type = TransformTransactionType(stmt.kind);
	auto info = make_uniq<TransactionInfo>(type);
	info->modifier = TransformTransactionModifier(stmt.transaction_type);
	return make_uniq<TransactionStatement>(std::move(info));
}

} // namespace goose
