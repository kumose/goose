#include <goose/parser/statement/select_statement.h>
#include <goose/parser/transformer.h>
#include <goose/common/string_util.h>

namespace goose {

unique_ptr<QueryNode> Transformer::TransformSelectNode(cantor::PGNode &node, bool is_select) {
	switch (node.type) {
	case cantor::T_PGVariableShowSelectStmt:
		return TransformShowSelect(PGCast<cantor::PGVariableShowSelectStmt>(node));
	case cantor::T_PGVariableShowStmt:
		return TransformShow(PGCast<cantor::PGVariableShowStmt>(node));
	default:
		return TransformSelectNodeInternal(PGCast<cantor::PGSelectStmt>(node), is_select);
	}
}

unique_ptr<QueryNode> Transformer::TransformSelectNodeInternal(cantor::PGSelectStmt &select,
                                                               bool is_select) {
	// Both Insert/Create Table As uses this.
	if (is_select) {
		if (select.intoClause) {
			throw ParserException("SELECT INTO not supported!");
		}
		if (select.lockingClause) {
			throw ParserException("SELECT locking clause is not supported!");
		}
	}
	if (select.pivot) {
		return TransformPivotStatement(select);
	}
	return TransformSelectInternal(select);
}

unique_ptr<SelectStatement> Transformer::TransformSelectStmt(cantor::PGSelectStmt &select, bool is_select) {
	auto result = make_uniq<SelectStatement>();
	result->node = TransformSelectNodeInternal(select, is_select);
	return result;
}

unique_ptr<SelectStatement> Transformer::TransformSelectStmt(cantor::PGNode &node, bool is_select) {
	auto select_node = TransformSelectNode(node, is_select);
	auto select_statement = make_uniq<SelectStatement>();
	select_statement->node = std::move(select_node);
	return select_statement;
}

} // namespace goose
