#include <goose/common/exception.h>
#include <goose/parser/expression/default_expression.h>
#include <goose/parser/transformer.h>

namespace goose {

unique_ptr<ParsedExpression> Transformer::TransformResTarget(cantor::PGResTarget &root) {
	auto expr = TransformExpression(root.val);
	if (!expr) {
		return nullptr;
	}
	if (root.name) {
		expr->SetAlias(root.name);
	}
	return expr;
}

unique_ptr<ParsedExpression> Transformer::TransformNamedArg(cantor::PGNamedArgExpr &root) {
	auto expr = TransformExpression(PGPointerCast<cantor::PGNode>(root.arg));
	if (root.name) {
		expr->SetAlias(root.name);
	}
	return expr;
}

unique_ptr<ParsedExpression> Transformer::TransformExpression(cantor::PGNode &node) {
	auto stack_checker = StackCheck();

	switch (node.type) {
	case cantor::T_PGColumnRef:
		return TransformColumnRef(PGCast<cantor::PGColumnRef>(node));
	case cantor::T_PGAConst:
		return TransformConstant(PGCast<cantor::PGAConst>(node));
	case cantor::T_PGAExpr:
		return TransformAExpr(PGCast<cantor::PGAExpr>(node));
	case cantor::T_PGFuncCall:
		return TransformFuncCall(PGCast<cantor::PGFuncCall>(node));
	case cantor::T_PGBoolExpr:
		return TransformBoolExpr(PGCast<cantor::PGBoolExpr>(node));
	case cantor::T_PGTypeCast:
		return TransformTypeCast(PGCast<cantor::PGTypeCast>(node));
	case cantor::T_PGCaseExpr:
		return TransformCase(PGCast<cantor::PGCaseExpr>(node));
	case cantor::T_PGSubLink:
		return TransformSubquery(PGCast<cantor::PGSubLink>(node));
	case cantor::T_PGCoalesceExpr:
		return TransformCoalesce(PGCast<cantor::PGAExpr>(node));
	case cantor::T_PGNullTest:
		return TransformNullTest(PGCast<cantor::PGNullTest>(node));
	case cantor::T_PGResTarget:
		return TransformResTarget(PGCast<cantor::PGResTarget>(node));
	case cantor::T_PGParamRef:
		return TransformParamRef(PGCast<cantor::PGParamRef>(node));
	case cantor::T_PGNamedArgExpr:
		return TransformNamedArg(PGCast<cantor::PGNamedArgExpr>(node));
	case cantor::T_PGSQLValueFunction:
		return TransformSQLValueFunction(PGCast<cantor::PGSQLValueFunction>(node));
	case cantor::T_PGSetToDefault:
		return make_uniq<DefaultExpression>();
	case cantor::T_PGCollateClause:
		return TransformCollateExpr(PGCast<cantor::PGCollateClause>(node));
	case cantor::T_PGIntervalConstant:
		return TransformInterval(PGCast<cantor::PGIntervalConstant>(node));
	case cantor::T_PGLambdaFunction:
		return TransformLambda(PGCast<cantor::PGLambdaFunction>(node));
	case cantor::T_PGSingleArrowFunction:
		return TransformSingleArrow(PGCast<cantor::PGSingleArrowFunction>(node));
	case cantor::T_PGAIndirection:
		return TransformArrayAccess(PGCast<cantor::PGAIndirection>(node));
	case cantor::T_PGPositionalReference:
		return TransformPositionalReference(PGCast<cantor::PGPositionalReference>(node));
	case cantor::T_PGGroupingFunc:
		return TransformGroupingFunction(PGCast<cantor::PGGroupingFunc>(node));
	case cantor::T_PGAStar:
		return TransformStarExpression(PGCast<cantor::PGAStar>(node));
	case cantor::T_PGBooleanTest:
		return TransformBooleanTest(PGCast<cantor::PGBooleanTest>(node));
	case cantor::T_PGMultiAssignRef:
		return TransformMultiAssignRef(PGCast<cantor::PGMultiAssignRef>(node));
	default:
		throw NotImplementedException("Expression type %s (%d)", NodetypeToString(node.type), (int)node.type);
	}
}

unique_ptr<ParsedExpression> Transformer::TransformExpression(optional_ptr<cantor::PGNode> node) {
	if (!node) {
		return nullptr;
	}
	return TransformExpression(*node);
}

void Transformer::TransformExpressionList(cantor::PGList &list,
                                          vector<unique_ptr<ParsedExpression>> &result) {
	for (auto node = list.head; node != nullptr; node = node->next) {
		auto target = PGPointerCast<cantor::PGNode>(node->data.ptr_value);

		auto expr = TransformExpression(*target);
		result.push_back(std::move(expr));
	}
}

} // namespace goose
