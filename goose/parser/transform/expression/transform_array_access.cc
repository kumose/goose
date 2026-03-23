#include <goose/common/exception.h>
#include <goose/common/exception/parser_exception.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/expression/function_expression.h>
#include <goose/parser/expression/operator_expression.h>
#include <goose/parser/transformer.h>

namespace goose {

unique_ptr<ParsedExpression> Transformer::TransformArrayAccess(cantor::PGAIndirection &indirection_node) {
	// Transform the source expression.
	unique_ptr<ParsedExpression> result;
	result = TransformExpression(indirection_node.arg);

	// Iterate the indices.
	// For more complex expressions like (foo).field_name[42] a single indirection
	// node can contain multiple indices.
	idx_t list_size = 0;
	for (auto node = indirection_node.indirection->head; node != nullptr; node = node->next) {
		auto target = PGPointerCast<cantor::PGNode>(node->data.ptr_value);

		switch (target->type) {
		case cantor::T_PGAIndices: {
			// Index access.
			auto indices = PGCast<cantor::PGAIndices>(*target.get());
			vector<unique_ptr<ParsedExpression>> children;
			children.push_back(std::move(result));

			if (indices.is_slice) {
				// If either the lower or upper bound is not specified, we use an empty constant LIST,
				// which we handle in the execution.
				auto constant_list = make_uniq<ConstantExpression>(Value::LIST(LogicalType::INTEGER, vector<Value>()));

				auto lower = indices.lidx ? TransformExpression(indices.lidx) : constant_list->Copy();
				children.push_back(std::move(lower));
				auto upper = indices.uidx ? TransformExpression(indices.uidx) : constant_list->Copy();
				children.push_back(std::move(upper));

				if (indices.step) {
					children.push_back(TransformExpression(indices.step));
				}
				result = make_uniq<OperatorExpression>(ExpressionType::ARRAY_SLICE, std::move(children));
				break;
			}

			// Array access.
			D_ASSERT(!indices.lidx && indices.uidx);
			children.push_back(TransformExpression(indices.uidx));
			result = make_uniq<OperatorExpression>(ExpressionType::ARRAY_EXTRACT, std::move(children));
			break;
		}
		case cantor::T_PGString: {
			auto value = PGCast<cantor::PGValue>(*target.get());
			vector<unique_ptr<ParsedExpression>> children;
			children.push_back(std::move(result));
			children.push_back(TransformValue(value));
			result = make_uniq<OperatorExpression>(ExpressionType::STRUCT_EXTRACT, std::move(children));
			break;
		}
		case cantor::T_PGFuncCall: {
			auto func = PGCast<cantor::PGFuncCall>(*target.get());
			auto function = TransformFuncCall(func);
			if (function->GetExpressionType() != ExpressionType::FUNCTION) {
				throw ParserException("%s.%s() call must be a function", result->ToString(), function->ToString());
			}
			auto &function_expr = function->Cast<FunctionExpression>();
			function_expr.children.insert(function_expr.children.begin(), std::move(result));
			result = std::move(function);
			break;
		}
		default:
			throw NotImplementedException("Unimplemented subscript type");
		}

		list_size++;
		StackCheck(list_size);
	}
	return result;
}

} // namespace goose
