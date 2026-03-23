#include <goose/common/type_visitor.h>
#include <goose/common/types.h>
#include <goose/common/helper.h>
#include <goose/common/operator/cast_operators.h>
#include <goose/function/cast/cast_function_set.h>
#include <goose/main/capi/capi_internal.h>

namespace goose {
struct CCastExecuteInfo {
	CastParameters &parameters;
	string error_message;

	explicit CCastExecuteInfo(CastParameters &parameters) : parameters(parameters), error_message() {
	}
};

struct CCastFunction {
	unique_ptr<LogicalType> source_type;
	unique_ptr<LogicalType> target_type;
	int64_t implicit_cast_cost = -1;

	goose_cast_function_t function;
	goose_function_info extra_info = nullptr;
	goose_delete_callback_t delete_callback = nullptr;
};

struct CCastFunctionUserData {
	goose_function_info data_ptr = nullptr;
	goose_delete_callback_t delete_callback = nullptr;

	CCastFunctionUserData(goose_function_info data_ptr_p, goose_delete_callback_t delete_callback_p)
	    : data_ptr(data_ptr_p), delete_callback(delete_callback_p) {
	}

	~CCastFunctionUserData() {
		if (data_ptr && delete_callback) {
			delete_callback(data_ptr);
		}
		data_ptr = nullptr;
		delete_callback = nullptr;
	}
};

struct CCastFunctionData final : public BoundCastData {
	goose_cast_function_t function;
	shared_ptr<CCastFunctionUserData> extra_info;

	explicit CCastFunctionData(goose_cast_function_t function_p, shared_ptr<CCastFunctionUserData> extra_info_p)
	    : function(function_p), extra_info(std::move(extra_info_p)) {
	}

	unique_ptr<BoundCastData> Copy() const override {
		return make_uniq<CCastFunctionData>(function, extra_info);
	}
};

static bool CAPICastFunction(Vector &input, Vector &output, idx_t count, CastParameters &parameters) {
	const auto is_const = input.GetVectorType() == VectorType::CONSTANT_VECTOR;
	input.Flatten(count);

	CCastExecuteInfo exec_info(parameters);
	const auto &data = parameters.cast_data->Cast<CCastFunctionData>();

	auto c_input = reinterpret_cast<goose_vector>(&input);
	auto c_output = reinterpret_cast<goose_vector>(&output);
	auto c_info = reinterpret_cast<goose_function_info>(&exec_info);

	const auto success = data.function(c_info, count, c_input, c_output);

	if (!success) {
		HandleCastError::AssignError(exec_info.error_message, parameters);
	}

	if (is_const && count == 1 && (success || !parameters.strict)) {
		output.SetVectorType(VectorType::CONSTANT_VECTOR);
	}

	return success;
}

} // namespace goose

goose_cast_function goose_create_cast_function() {
	const auto function = new goose::CCastFunction();
	return reinterpret_cast<goose_cast_function>(function);
}

void goose_cast_function_set_source_type(goose_cast_function cast_function, goose_logical_type source_type) {
	if (!cast_function || !source_type) {
		return;
	}
	const auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(source_type));
	auto &cast = *(reinterpret_cast<goose::CCastFunction *>(cast_function));
	cast.source_type = goose::make_uniq<goose::LogicalType>(logical_type);
}

void goose_cast_function_set_target_type(goose_cast_function cast_function, goose_logical_type target_type) {
	if (!cast_function || !target_type) {
		return;
	}
	const auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(target_type));
	auto &cast = *(reinterpret_cast<goose::CCastFunction *>(cast_function));
	cast.target_type = goose::make_uniq<goose::LogicalType>(logical_type);
}

void goose_cast_function_set_implicit_cast_cost(goose_cast_function cast_function, int64_t cost) {
	if (!cast_function) {
		return;
	}
	auto &custom_type = *(reinterpret_cast<goose::CCastFunction *>(cast_function));
	custom_type.implicit_cast_cost = cost;
}

void goose_cast_function_set_function(goose_cast_function cast_function, goose_cast_function_t function) {
	if (!cast_function || !function) {
		return;
	}
	auto &cast = *(reinterpret_cast<goose::CCastFunction *>(cast_function));
	cast.function = function;
}

goose_cast_mode goose_cast_function_get_cast_mode(goose_function_info info) {
	const auto &cast_info = *reinterpret_cast<goose::CCastExecuteInfo *>(info);
	return cast_info.parameters.error_message == nullptr ? GOOSE_CAST_NORMAL : GOOSE_CAST_TRY;
}

void goose_cast_function_set_error(goose_function_info info, const char *error) {
	auto &cast_info = *reinterpret_cast<goose::CCastExecuteInfo *>(info);
	cast_info.error_message = error;
}

void goose_cast_function_set_row_error(goose_function_info info, const char *error, idx_t row, goose_vector output) {
	auto &cast_info = *reinterpret_cast<goose::CCastExecuteInfo *>(info);
	cast_info.error_message = error;
	if (!output) {
		return;
	}
	auto &output_vector = *reinterpret_cast<goose::Vector *>(output);
	goose::FlatVector::SetNull(output_vector, row, true);
}

void goose_cast_function_set_extra_info(goose_cast_function cast_function, void *extra_info,
                                         goose_delete_callback_t destroy) {
	if (!cast_function || !extra_info) {
		return;
	}
	auto &cast = *reinterpret_cast<goose::CCastFunction *>(cast_function);
	cast.extra_info = static_cast<goose_function_info>(extra_info);
	cast.delete_callback = destroy;
}

void *goose_cast_function_get_extra_info(goose_function_info info) {
	if (!info) {
		return nullptr;
	}
	auto &cast_info = *reinterpret_cast<goose::CCastExecuteInfo *>(info);
	auto &cast_data = cast_info.parameters.cast_data->Cast<goose::CCastFunctionData>();
	return cast_data.extra_info->data_ptr;
}

goose_state goose_register_cast_function(goose_connection connection, goose_cast_function cast_function) {
	if (!connection || !cast_function) {
		return GooseError;
	}
	auto &cast = *reinterpret_cast<goose::CCastFunction *>(cast_function);
	if (!cast.source_type || !cast.target_type || !cast.function) {
		return GooseError;
	}

	const auto &source_type = *cast.source_type;
	const auto &target_type = *cast.target_type;

	if (goose::TypeVisitor::Contains(source_type, goose::LogicalTypeId::INVALID) ||
	    goose::TypeVisitor::Contains(source_type, goose::LogicalTypeId::ANY)) {
		return GooseError;
	}

	if (goose::TypeVisitor::Contains(target_type, goose::LogicalTypeId::INVALID) ||
	    goose::TypeVisitor::Contains(target_type, goose::LogicalTypeId::ANY)) {
		return GooseError;
	}

	try {
		const auto con = reinterpret_cast<goose::Connection *>(connection);
		con->context->RunFunctionInTransaction([&]() {
			auto &config = goose::DBConfig::GetConfig(*con->context);
			auto &casts = config.GetCastFunctions();

			auto extra_info =
			    goose::make_shared_ptr<goose::CCastFunctionUserData>(cast.extra_info, cast.delete_callback);
			auto cast_data = goose::make_uniq<goose::CCastFunctionData>(cast.function, std::move(extra_info));
			goose::BoundCastInfo cast_info(goose::CAPICastFunction, std::move(cast_data));
			casts.RegisterCastFunction(source_type, target_type, std::move(cast_info), cast.implicit_cast_cost);
		});
	} catch (...) {
		return GooseError;
	}
	return GooseSuccess;
}

void goose_destroy_cast_function(goose_cast_function *cast_function) {
	if (!cast_function || !*cast_function) {
		return;
	}
	const auto function = reinterpret_cast<goose::CCastFunction *>(*cast_function);
	delete function;
	*cast_function = nullptr;
}
