#include <goose/common/vector_operations/vector_operations.h>
#include <goose/execution/expression_executor.h>
#include <goose/planner/expression/bound_parameter_expression.h>

namespace goose {

unique_ptr<ExpressionState> ExpressionExecutor::InitializeState(const BoundParameterExpression &expr,
                                                                ExpressionExecutorState &root) {
	auto result = make_uniq<ExpressionState>(expr, root);
	result->Finalize();
	return result;
}

void ExpressionExecutor::Execute(const BoundParameterExpression &expr, ExpressionState *state,
                                 const SelectionVector *sel, idx_t count, Vector &result) {
	D_ASSERT(expr.parameter_data);
	D_ASSERT(expr.parameter_data->return_type == expr.return_type);
	D_ASSERT(expr.parameter_data->GetValue().type() == expr.return_type);
	result.Reference(expr.parameter_data->GetValue());
}

} // namespace goose
