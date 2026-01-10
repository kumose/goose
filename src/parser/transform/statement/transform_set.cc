#include <goose/parser/statement/set_statement.h>

#include <goose/parser/transformer.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/expression/columnref_expression.h>

namespace goose {

namespace {

SetScope ToSetScope(cantor::VariableSetScope pg_scope) {
	switch (pg_scope) {
	case cantor::VariableSetScope::VAR_SET_SCOPE_LOCAL:
		return SetScope::LOCAL;
	case cantor::VariableSetScope::VAR_SET_SCOPE_SESSION:
		return SetScope::SESSION;
	case cantor::VariableSetScope::VAR_SET_SCOPE_GLOBAL:
		return SetScope::GLOBAL;
	case cantor::VariableSetScope::VAR_SET_SCOPE_VARIABLE:
		return SetScope::VARIABLE;
	case cantor::VariableSetScope::VAR_SET_SCOPE_DEFAULT:
		return SetScope::AUTOMATIC;
	default:
		throw InternalException("Unexpected pg_scope: %d", pg_scope);
	}
}

SetType ToSetType(cantor::VariableSetKind pg_kind) {
	switch (pg_kind) {
	case cantor::VariableSetKind::VAR_SET_VALUE:
		return SetType::SET;
	case cantor::VariableSetKind::VAR_RESET:
		return SetType::RESET;
	default:
		throw NotImplementedException("Can only SET or RESET a variable");
	}
}

} // namespace

unique_ptr<SetStatement> Transformer::TransformSetVariable(cantor::PGVariableSetStmt &stmt) {
	if (stmt.scope == cantor::VariableSetScope::VAR_SET_SCOPE_LOCAL) {
		throw NotImplementedException("SET LOCAL is not implemented.");
	}

	string name(stmt.name);
	D_ASSERT(!name.empty()); // parser protect us!
	if (stmt.args->length != 1) {
		throw ParserException("SET needs a single scalar value parameter");
	}
	auto scope = ToSetScope(stmt.scope);
	D_ASSERT(stmt.args->head && stmt.args->head->data.ptr_value);
	auto const_val = PGPointerCast<cantor::PGNode>(stmt.args->head->data.ptr_value);
	auto expr = TransformExpression(const_val);
	if (expr->GetExpressionType() == ExpressionType::COLUMN_REF) {
		auto &colref = expr->Cast<ColumnRefExpression>();
		Value val;
		if (!colref.IsQualified()) {
			val = Value(colref.GetColumnName());
		} else {
			val = Value(expr->ToString());
		}
		expr = make_uniq<ConstantExpression>(std::move(val));
	}
	if (expr->GetExpressionType() == ExpressionType::VALUE_DEFAULT) {
		// set to default = reset
		return make_uniq<ResetVariableStatement>(std::move(name), scope);
	}
	return make_uniq<SetVariableStatement>(std::move(name), std::move(expr), scope);
}

unique_ptr<SetStatement> Transformer::TransformResetVariable(cantor::PGVariableSetStmt &stmt) {
	D_ASSERT(stmt.kind == cantor::VariableSetKind::VAR_RESET);

	if (stmt.scope == cantor::VariableSetScope::VAR_SET_SCOPE_LOCAL) {
		throw NotImplementedException("RESET LOCAL is not implemented.");
	}

	string name(stmt.name);
	D_ASSERT(!name.empty()); // parser protect us!

	return make_uniq<ResetVariableStatement>(name, ToSetScope(stmt.scope));
}

unique_ptr<SetStatement> Transformer::TransformSet(cantor::PGVariableSetStmt &stmt) {
	D_ASSERT(stmt.type == cantor::T_PGVariableSetStmt);

	SetType set_type = ToSetType(stmt.kind);
	switch (set_type) {
	case SetType::SET:
		return TransformSetVariable(stmt);
	case SetType::RESET:
		return TransformResetVariable(stmt);
	default:
		throw InternalException("Type not implemented for SetType");
	}
}

} // namespace goose
