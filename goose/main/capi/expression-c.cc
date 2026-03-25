#include <goose/main/capi/capi_internal.h>

#include <goose/execution/expression_executor.h>

using goose::CClientContextWrapper;
using goose::ExpressionWrapper;

void goose_destroy_expression(goose_expression *expr) {
	if (!expr || !*expr) {
		return;
	}
	auto wrapper = reinterpret_cast<ExpressionWrapper *>(*expr);
	delete wrapper;
	*expr = nullptr;
}

goose_logical_type goose_expression_return_type(goose_expression expr) {
	if (!expr) {
		return nullptr;
	}
	auto wrapper = reinterpret_cast<ExpressionWrapper *>(expr);
	auto logical_type = new goose::LogicalType(wrapper->expr->return_type);
	return reinterpret_cast<goose_logical_type>(logical_type);
}

bool goose_expression_is_foldable(goose_expression expr) {
	if (!expr) {
		return false;
	}
	auto wrapper = reinterpret_cast<ExpressionWrapper *>(expr);
	return wrapper->expr->IsFoldable();
}

goose_error_data goose_expression_fold(goose_client_context context, goose_expression expr,
                                         goose_value *out_value) {
	if (!expr || !goose_expression_is_foldable(expr)) {
		return nullptr;
	}

	auto value = new goose::Value;
	try {
		auto context_wrapper = reinterpret_cast<CClientContextWrapper *>(context);
		auto expr_wrapper = reinterpret_cast<ExpressionWrapper *>(expr);
		*value = goose::ExpressionExecutor::EvaluateScalar(context_wrapper->context, *expr_wrapper->expr);
		*out_value = reinterpret_cast<goose_value>(value);
	} catch (const goose::Exception &ex) {
		delete value;
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, ex.what());
	} catch (const std::exception &ex) {
		delete value;
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, ex.what());
	} catch (...) {
		delete value;
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, "unknown error occurred during folding");
	}
	return nullptr;
}
