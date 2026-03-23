#include <goose/parser/statement/call_statement.h>
#include <goose/parser/transformer.h>

namespace goose {

unique_ptr<CallStatement> Transformer::TransformCall(cantor::PGCallStmt &stmt) {
	auto result = make_uniq<CallStatement>();
	result->function = TransformFuncCall(*PGPointerCast<cantor::PGFuncCall>(stmt.func));
	return result;
}

} // namespace goose
