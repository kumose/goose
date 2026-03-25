#include <goose/catalog/catalog.h>
#include <goose/common/type_visitor.h>
#include <goose/common/types.h>
#include <goose/function/function.h>
#include <goose/function/scalar_function.h>
#include <goose/main/capi/capi_internal.h>
#include <goose/main/client_context.h>
#include <goose/parser/parsed_data/create_aggregate_function_info.h>
#include <goose/planner/expression/bound_function_expression.h>

namespace goose {

struct CAggregateFunctionInfo : public AggregateFunctionInfo {
	~CAggregateFunctionInfo() override {
		if (extra_info && delete_callback) {
			delete_callback(extra_info);
		}
		extra_info = nullptr;
		delete_callback = nullptr;
	}

	goose_aggregate_state_size state_size = nullptr;
	goose_aggregate_init_t state_init = nullptr;
	goose_aggregate_update_t update = nullptr;
	goose_aggregate_combine_t combine = nullptr;
	goose_aggregate_finalize_t finalize = nullptr;
	goose_aggregate_destroy_t destroy = nullptr;
	goose_function_info extra_info = nullptr;
	goose_delete_callback_t delete_callback = nullptr;
};

struct CAggregateExecuteInfo {
	explicit CAggregateExecuteInfo(CAggregateFunctionInfo &info) : info(info) {
	}

	CAggregateFunctionInfo &info;
	bool success = true;
	string error;
};

struct CAggregateFunctionBindData : public FunctionData {
	explicit CAggregateFunctionBindData(CAggregateFunctionInfo &info) : info(info) {
	}

	unique_ptr<FunctionData> Copy() const override {
		return make_uniq<CAggregateFunctionBindData>(info);
	}
	bool Equals(const FunctionData &other_p) const override {
		auto &other = other_p.Cast<CAggregateFunctionBindData>();
		return info.extra_info == other.info.extra_info && info.update == other.info.update &&
		       info.combine == other.info.combine && info.finalize == other.info.finalize;
	}

	CAggregateFunctionInfo &info;
};

goose::AggregateFunction &GetCAggregateFunction(goose_aggregate_function function) {
	return *reinterpret_cast<goose::AggregateFunction *>(function);
}

goose::AggregateFunctionSet &GetCAggregateFunctionSet(goose_aggregate_function_set function_set) {
	return *reinterpret_cast<goose::AggregateFunctionSet *>(function_set);
}

unique_ptr<FunctionData> CAPIAggregateBind(ClientContext &context, AggregateFunction &function,
                                           vector<unique_ptr<Expression>> &arguments) {
	auto &info = function.function_info->Cast<CAggregateFunctionInfo>();
	return make_uniq<CAggregateFunctionBindData>(info);
}

idx_t CAPIAggregateStateSize(const AggregateFunction &function) {
	auto &function_info = function.function_info->Cast<goose::CAggregateFunctionInfo>();
	CAggregateExecuteInfo exec_info(function_info);
	auto c_function_info = reinterpret_cast<goose_function_info>(&exec_info);
	auto result = function_info.state_size(c_function_info);
	if (!exec_info.success) {
		throw InvalidInputException(exec_info.error);
	}
	return result;
}

void CAPIAggregateStateInit(const AggregateFunction &function, data_ptr_t state) {
	auto &function_info = function.function_info->Cast<goose::CAggregateFunctionInfo>();
	CAggregateExecuteInfo exec_info(function_info);
	auto c_function_info = reinterpret_cast<goose_function_info>(&exec_info);
	function_info.state_init(c_function_info, reinterpret_cast<goose_aggregate_state>(state));
	if (!exec_info.success) {
		throw InvalidInputException(exec_info.error);
	}
}

void CAPIAggregateUpdate(Vector inputs[], AggregateInputData &aggr_input_data, idx_t input_count, Vector &state,
                         idx_t count) {
	DataChunk chunk;
	for (idx_t c = 0; c < input_count; c++) {
		inputs[c].Flatten(count);
		chunk.data.emplace_back(inputs[c]);
	}
	chunk.SetCardinality(count);

	auto &bind_data = aggr_input_data.bind_data->Cast<CAggregateFunctionBindData>();
	auto state_data = FlatVector::GetDataUnsafe<goose_aggregate_state>(state);
	auto c_input_chunk = reinterpret_cast<goose_data_chunk>(&chunk);

	CAggregateExecuteInfo exec_info(bind_data.info);
	auto c_function_info = reinterpret_cast<goose_function_info>(&exec_info);
	bind_data.info.update(c_function_info, c_input_chunk, state_data);
	if (!exec_info.success) {
		throw InvalidInputException(exec_info.error);
	}
}

void CAPIAggregateCombine(Vector &state, Vector &combined, AggregateInputData &aggr_input_data, idx_t count) {
	state.Flatten(count);
	auto &bind_data = aggr_input_data.bind_data->Cast<CAggregateFunctionBindData>();
	auto input_state_data = FlatVector::GetDataUnsafe<goose_aggregate_state>(state);
	auto result_state_data = FlatVector::GetDataUnsafe<goose_aggregate_state>(combined);
	CAggregateExecuteInfo exec_info(bind_data.info);
	auto c_function_info = reinterpret_cast<goose_function_info>(&exec_info);
	bind_data.info.combine(c_function_info, input_state_data, result_state_data, count);
	if (!exec_info.success) {
		throw InvalidInputException(exec_info.error);
	}
}

void CAPIAggregateFinalize(Vector &state, AggregateInputData &aggr_input_data, Vector &result, idx_t count,
                           idx_t offset) {
	state.Flatten(count);
	auto &bind_data = aggr_input_data.bind_data->Cast<CAggregateFunctionBindData>();
	auto input_state_data = FlatVector::GetDataUnsafe<goose_aggregate_state>(state);
	auto result_vector = reinterpret_cast<goose_vector>(&result);

	CAggregateExecuteInfo exec_info(bind_data.info);
	auto c_function_info = reinterpret_cast<goose_function_info>(&exec_info);
	bind_data.info.finalize(c_function_info, input_state_data, result_vector, count, offset);
	if (!exec_info.success) {
		throw InvalidInputException(exec_info.error);
	}
}

void CAPIAggregateDestructor(Vector &state, AggregateInputData &aggr_input_data, idx_t count) {
	auto &bind_data = aggr_input_data.bind_data->Cast<CAggregateFunctionBindData>();
	auto input_state_data = FlatVector::GetDataUnsafe<goose_aggregate_state>(state);
	bind_data.info.destroy(input_state_data, count);
}

} // namespace goose

using goose::GetCAggregateFunction;

goose_aggregate_function goose_create_aggregate_function() {
	auto function = new goose::AggregateFunction("", {}, goose::LogicalType::INVALID, goose::CAPIAggregateStateSize,
	                                              goose::CAPIAggregateStateInit, goose::CAPIAggregateUpdate,
	                                              goose::CAPIAggregateCombine, goose::CAPIAggregateFinalize, nullptr,
	                                              goose::CAPIAggregateBind);
	try {
		function->function_info = goose::make_shared_ptr<goose::CAggregateFunctionInfo>();
		return reinterpret_cast<goose_aggregate_function>(function);
	} catch (...) {
		delete function;
		return nullptr;
	}
}

void goose_destroy_aggregate_function(goose_aggregate_function *function) {
	if (function && *function) {
		auto aggregate_function = reinterpret_cast<goose::AggregateFunction *>(*function);
		delete aggregate_function;
		*function = nullptr;
	}
}

void goose_aggregate_function_set_name(goose_aggregate_function function, const char *name) {
	if (!function || !name) {
		return;
	}
	auto &aggregate_function = GetCAggregateFunction(function);
	aggregate_function.name = name;
}

void goose_aggregate_function_add_parameter(goose_aggregate_function function, goose_logical_type type) {
	if (!function || !type) {
		return;
	}
	auto &aggregate_function = GetCAggregateFunction(function);
	auto logical_type = reinterpret_cast<goose::LogicalType *>(type);
	aggregate_function.arguments.push_back(*logical_type);
}

void goose_aggregate_function_set_return_type(goose_aggregate_function function, goose_logical_type type) {
	if (!function || !type) {
		return;
	}
	auto &aggregate_function = GetCAggregateFunction(function);
	auto logical_type = reinterpret_cast<goose::LogicalType *>(type);
	aggregate_function.SetReturnType(*logical_type);
}

void goose_aggregate_function_set_functions(goose_aggregate_function function, goose_aggregate_state_size state_size,
                                             goose_aggregate_init_t state_init, goose_aggregate_update_t update,
                                             goose_aggregate_combine_t combine, goose_aggregate_finalize_t finalize) {
	if (!function || !state_size || !state_init || !update || !combine || !finalize) {
		return;
	}
	auto &aggregate_function = GetCAggregateFunction(function);
	auto &function_info = aggregate_function.function_info->Cast<goose::CAggregateFunctionInfo>();
	function_info.state_size = state_size;
	function_info.state_init = state_init;
	function_info.update = update;
	function_info.combine = combine;
	function_info.finalize = finalize;
}

void goose_aggregate_function_set_destructor(goose_aggregate_function function, goose_aggregate_destroy_t destroy) {
	if (!function || !destroy) {
		return;
	}
	auto &aggregate_function = GetCAggregateFunction(function);
	auto &function_info = aggregate_function.function_info->Cast<goose::CAggregateFunctionInfo>();
	function_info.destroy = destroy;
	aggregate_function.SetStateDestructorCallback(goose::CAPIAggregateDestructor);
}

goose_state goose_register_aggregate_function(goose_connection connection, goose_aggregate_function function) {
	if (!connection || !function) {
		return GooseError;
	}

	auto &aggregate_function = GetCAggregateFunction(function);
	goose::AggregateFunctionSet set(aggregate_function.name);
	set.AddFunction(aggregate_function);
	return goose_register_aggregate_function_set(connection, reinterpret_cast<goose_aggregate_function_set>(&set));
}

void goose_aggregate_function_set_special_handling(goose_aggregate_function function) {
	if (!function) {
		return;
	}
	auto &aggregate_function = GetCAggregateFunction(function);
	aggregate_function.SetNullHandling(goose::FunctionNullHandling::SPECIAL_HANDLING);
}

void goose_aggregate_function_set_extra_info(goose_aggregate_function function, void *extra_info,
                                              goose_delete_callback_t destroy) {
	if (!function || !extra_info) {
		return;
	}
	auto &aggregate_function = GetCAggregateFunction(function);
	auto &function_info = aggregate_function.function_info->Cast<goose::CAggregateFunctionInfo>();
	function_info.extra_info = static_cast<goose_function_info>(extra_info);
	function_info.delete_callback = destroy;
}

goose::CAggregateExecuteInfo &GetCAggregateExecuteInfo(goose_function_info info) {
	D_ASSERT(info);
	return *reinterpret_cast<goose::CAggregateExecuteInfo *>(info);
}

void *goose_aggregate_function_get_extra_info(goose_function_info info_p) {
	auto &exec_info = GetCAggregateExecuteInfo(info_p);
	return exec_info.info.extra_info;
}

void goose_aggregate_function_set_error(goose_function_info info_p, const char *error) {
	auto &exec_info = GetCAggregateExecuteInfo(info_p);
	exec_info.error = error;
	exec_info.success = false;
}

goose_aggregate_function_set goose_create_aggregate_function_set(const char *name) {
	if (!name || !*name) {
		return nullptr;
	}
	try {
		auto function_set = new goose::AggregateFunctionSet(name);
		return reinterpret_cast<goose_aggregate_function_set>(function_set);
	} catch (...) {
		return nullptr;
	}
}

void goose_destroy_aggregate_function_set(goose_aggregate_function_set *set) {
	if (set && *set) {
		auto aggregate_function_set = reinterpret_cast<goose::AggregateFunctionSet *>(*set);
		delete aggregate_function_set;
		*set = nullptr;
	}
}

goose_state goose_add_aggregate_function_to_set(goose_aggregate_function_set set,
                                                  goose_aggregate_function function) {
	if (!set || !function) {
		return GooseError;
	}
	auto &aggregate_function_set = goose::GetCAggregateFunctionSet(set);
	auto &aggregate_function = GetCAggregateFunction(function);
	aggregate_function_set.AddFunction(aggregate_function);
	return GooseSuccess;
}

goose_state goose_register_aggregate_function_set(goose_connection connection,
                                                    goose_aggregate_function_set function_set) {
	if (!connection || !function_set) {
		return GooseError;
	}
	auto &set = goose::GetCAggregateFunctionSet(function_set);
	for (idx_t idx = 0; idx < set.Size(); idx++) {
		auto &aggregate_function = set.GetFunctionReferenceByOffset(idx);
		auto &info = aggregate_function.function_info->Cast<goose::CAggregateFunctionInfo>();

		if (aggregate_function.name.empty() || !info.update || !info.combine || !info.finalize) {
			return GooseError;
		}
		if (goose::TypeVisitor::Contains(aggregate_function.GetReturnType(), goose::LogicalTypeId::INVALID) ||
		    goose::TypeVisitor::Contains(aggregate_function.GetReturnType(), goose::LogicalTypeId::ANY)) {
			return GooseError;
		}
		for (const auto &argument : aggregate_function.arguments) {
			if (goose::TypeVisitor::Contains(argument, goose::LogicalTypeId::INVALID)) {
				return GooseError;
			}
		}
	}

	try {
		auto con = reinterpret_cast<goose::Connection *>(connection);
		con->context->RunFunctionInTransaction([&]() {
			auto &catalog = goose::Catalog::GetSystemCatalog(*con->context);
			goose::CreateAggregateFunctionInfo sf_info(set);
			sf_info.on_conflict = goose::OnCreateConflict::ALTER_ON_CONFLICT;
			catalog.CreateFunction(*con->context, sf_info);
		});
	} catch (...) {
		return GooseError;
	}
	return GooseSuccess;
}
