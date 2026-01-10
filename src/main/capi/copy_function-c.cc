#include <goose/common/type_visitor.h>
#include <goose/common/types.h>
#include <goose/common/helper.h>
#include <goose/function/copy_function.h>
#include <goose/function/table_function.h>
#include <goose/main/capi/capi_internal.h>
#include <goose/main/capi/capi_internal_table.h>
#include <goose/parser/parsed_data/create_copy_function_info.h>

//----------------------------------------------------------------------------------------------------------------------
// Common Copy Function Info
//----------------------------------------------------------------------------------------------------------------------

namespace goose {
namespace {

struct CCopyFunctionInfo : public CopyFunctionInfo {
	~CCopyFunctionInfo() override {
		if (extra_info && delete_callback) {
			delete_callback(extra_info);
		}
		extra_info = nullptr;
		delete_callback = nullptr;
	}

	goose_copy_function_bind_t bind_to = nullptr;
	goose_copy_function_global_init_t global_init = nullptr;
	goose_copy_function_sink_t sink = nullptr;
	goose_copy_function_finalize_t finalize = nullptr;

	void *extra_info = nullptr;
	goose_delete_callback_t delete_callback = nullptr;
};

Value MakeValueFromCopyOptions(const case_insensitive_map_t<vector<Value>> &options) {
	child_list_t<goose::Value> option_list;
	for (auto &entry : options) {
		// Uppercase the option name, to make it simpler for users
		auto name = StringUtil::Upper(entry.first);
		auto &values = entry.second;

		if (values.empty()) {
			// Null!
			option_list.emplace_back(std::move(name), Value());
			continue;
		}
		if (values.size() == 1) {
			// Single value
			option_list.emplace_back(std::move(name), values[0]);
			continue;
		}

		auto is_same_type = true;
		auto first_type = values[0].type();
		for (auto &val : values) {
			if (val.type() != first_type) {
				// Different types, cannot unify
				is_same_type = false;
				break;
			}
		}

		// Is same type: create a list of that type
		if (is_same_type) {
			option_list.emplace_back(std::move(name), Value::LIST(first_type, values));
			continue;
		}

		// Different types: create an unnamed struct
		child_list_t<Value> children;
		for (auto &val : values) {
			children.emplace_back("", val);
		}
		option_list.emplace_back(std::move(name), Value::STRUCT(children));
	}

	if (option_list.empty()) {
		// No options
		return Value();
	}

	// Return a struct of all options
	return Value::STRUCT(std::move(option_list));
}

} // namespace
} // namespace goose

goose_copy_function goose_create_copy_function() {
	auto function = new goose::CopyFunction("");

	function->function_info = goose::make_shared_ptr<goose::CCopyFunctionInfo>();

	return reinterpret_cast<goose_copy_function>(function);
}

void goose_copy_function_set_name(goose_copy_function copy_function, const char *name) {
	if (!copy_function || !name) {
		return;
	}
	auto &copy_function_ref = *reinterpret_cast<goose::CopyFunction *>(copy_function);
	copy_function_ref.name = name;
}

void goose_destroy_copy_function(goose_copy_function *copy_function) {
	if (copy_function && *copy_function) {
		auto function = reinterpret_cast<goose::CopyFunction *>(*copy_function);
		delete function;
		*copy_function = nullptr;
	}
}

void goose_copy_function_set_extra_info(goose_copy_function function, void *extra_info,
                                         goose_delete_callback_t destroy) {
	if (!function) {
		return;
	}
	auto &copy_function_ref = *reinterpret_cast<goose::CopyFunction *>(function);
	auto &info = copy_function_ref.function_info->Cast<goose::CCopyFunctionInfo>();
	info.extra_info = extra_info;
	info.delete_callback = destroy;
}

//----------------------------------------------------------------------------------------------------------------------
// Copy To Bind
//----------------------------------------------------------------------------------------------------------------------
namespace goose {
namespace {
struct CCopyToBindInfo : FunctionData {
	shared_ptr<CopyFunctionInfo> function_info;
	void *bind_data = nullptr;
	goose_delete_callback_t delete_callback = nullptr;

	unique_ptr<FunctionData> Copy() const override {
		throw InternalException("CCopyToBindInfo cannot be copied");
	}

	bool Equals(const FunctionData &other_p) const override {
		auto &other = other_p.Cast<CCopyToBindInfo>();
		return bind_data == other.bind_data && delete_callback == other.delete_callback;
	}

	~CCopyToBindInfo() override {
		if (bind_data && delete_callback) {
			delete_callback(bind_data);
		}
		bind_data = nullptr;
		delete_callback = nullptr;
	}
};

struct CCopyFunctionToInternalBindInfo {
	CCopyFunctionToInternalBindInfo(ClientContext &context, CopyFunctionBindInput &input,
	                                const vector<LogicalType> &sql_types, const vector<string> &names,
	                                const CCopyFunctionInfo &function_info)
	    : context(context), input(input), sql_types(sql_types), names(names), function_info(function_info),
	      success(true) {
	}

	ClientContext &context;
	CopyFunctionBindInput &input;
	const vector<LogicalType> &sql_types;
	const vector<string> &names;
	const CCopyFunctionInfo &function_info;
	bool success;
	string error;

	// Supplied by the user
	void *bind_data = nullptr;
	goose_delete_callback_t delete_callback = nullptr;
};

unique_ptr<FunctionData> CCopyToBind(ClientContext &context, CopyFunctionBindInput &input, const vector<string> &names,
                                     const vector<LogicalType> &sql_types) {
	auto &info = input.function_info->Cast<CCopyFunctionInfo>();

	auto result = make_uniq<CCopyToBindInfo>();
	result->function_info = input.function_info;

	if (info.bind_to) {
		// Call the user-defined bind function
		CCopyFunctionToInternalBindInfo bind_info(context, input, sql_types, names, info);
		info.bind_to(reinterpret_cast<goose_copy_function_bind_info>(&bind_info));

		// Pass on user bind data to the result
		result->bind_data = bind_info.bind_data;
		result->delete_callback = bind_info.delete_callback;

		if (!bind_info.success) {
			throw BinderException(bind_info.error);
		}
	}
	return std::move(result);
}

} // namespace
} // namespace goose

void goose_copy_function_set_bind(goose_copy_function copy_function, goose_copy_function_bind_t bind) {
	if (!copy_function || !bind) {
		return;
	}

	auto &copy_function_ref = *reinterpret_cast<goose::CopyFunction *>(copy_function);
	auto &info = copy_function_ref.function_info->Cast<goose::CCopyFunctionInfo>();

	// Set C bind callback
	info.bind_to = bind;
}

void goose_copy_function_bind_set_error(goose_copy_function_bind_info info, const char *error) {
	if (!info || !error) {
		return;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyFunctionToInternalBindInfo *>(info);

	// Set the error message
	info_ref.error = error;
	info_ref.success = false;
}

void *goose_copy_function_bind_get_extra_info(goose_copy_function_bind_info info) {
	if (!info) {
		return nullptr;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyFunctionToInternalBindInfo *>(info);
	return info_ref.function_info.extra_info;
}

goose_client_context goose_copy_function_bind_get_client_context(goose_copy_function_bind_info info) {
	if (!info) {
		return nullptr;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyFunctionToInternalBindInfo *>(info);
	auto wrapper = new goose::CClientContextWrapper(info_ref.context);
	return reinterpret_cast<goose_client_context>(wrapper);
}

idx_t goose_copy_function_bind_get_column_count(goose_copy_function_bind_info info) {
	if (!info) {
		return 0;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyFunctionToInternalBindInfo *>(info);
	return info_ref.sql_types.size();
}

goose_logical_type goose_copy_function_bind_get_column_type(goose_copy_function_bind_info info, idx_t col_idx) {
	if (!info) {
		return nullptr;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyFunctionToInternalBindInfo *>(info);
	if (col_idx >= info_ref.sql_types.size()) {
		return nullptr;
	}
	return reinterpret_cast<goose_logical_type>(new goose::LogicalType(info_ref.sql_types[col_idx]));
}

goose_value goose_copy_function_bind_get_options(goose_copy_function_bind_info info) {
	if (!info) {
		return nullptr;
	}

	auto &info_ref = *reinterpret_cast<goose::CCopyFunctionToInternalBindInfo *>(info);
	auto &options = info_ref.input.info.options;

	// return as struct of options
	auto options_value = goose::MakeValueFromCopyOptions(options);
	return reinterpret_cast<goose_value>(new goose::Value(options_value));
}

void goose_copy_function_bind_set_bind_data(goose_copy_function_bind_info info, void *bind_data,
                                             goose_delete_callback_t destructor) {
	if (!info) {
		return;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyFunctionToInternalBindInfo *>(info);

	// Store the bind data and destructor
	info_ref.bind_data = bind_data;
	info_ref.delete_callback = destructor;
}

//----------------------------------------------------------------------------------------------------------------------
// Copy To Global Initialize
//----------------------------------------------------------------------------------------------------------------------
namespace goose {
namespace {

struct CCopyToGlobalState : GlobalFunctionData {
	void *global_state = nullptr;
	goose_delete_callback_t delete_callback = nullptr;

	~CCopyToGlobalState() override {
		if (global_state && delete_callback) {
			delete_callback(global_state);
		}
		global_state = nullptr;
		delete_callback = nullptr;
	}
};

struct CCopyToGlobalInitInfo {
	CCopyToGlobalInitInfo(ClientContext &context, FunctionData &bind_data, const string &file_path)
	    : context(context), bind_data(bind_data), file_path(file_path) {
	}

	ClientContext &context;
	FunctionData &bind_data;
	const string &file_path;

	string error;
	bool success = true;

	void *global_state = nullptr;
	goose_delete_callback_t delete_callback = nullptr;
};

unique_ptr<GlobalFunctionData> CCopyToGlobalInit(ClientContext &context, FunctionData &bind_data,
                                                 const string &file_path) {
	auto &bind_info = bind_data.Cast<CCopyToBindInfo>();
	auto &function_info = bind_info.function_info->Cast<CCopyFunctionInfo>();

	auto result = make_uniq<CCopyToGlobalState>();

	if (function_info.global_init) {
		// Call the user-defined global init function
		CCopyToGlobalInitInfo global_init_info(context, bind_data, file_path);
		function_info.global_init(reinterpret_cast<goose_copy_function_global_init_info>(&global_init_info));

		// Pass on user global state to the result
		result->global_state = global_init_info.global_state;
		result->delete_callback = global_init_info.delete_callback;

		if (!global_init_info.success) {
			throw InvalidInputException(global_init_info.error);
		}
	}

	return std::move(result);
}

} // namespace
} // namespace goose

void goose_copy_function_set_global_init(goose_copy_function copy_function, goose_copy_function_global_init_t init) {
	if (!copy_function || !init) {
		return;
	}
	auto &copy_function_ref = *reinterpret_cast<goose::CopyFunction *>(copy_function);
	auto &info = copy_function_ref.function_info->Cast<goose::CCopyFunctionInfo>();

	// Set C global init callback
	info.global_init = init;
}

void goose_copy_function_global_init_set_error(goose_copy_function_global_init_info info, const char *error) {
	if (!info || !error) {
		return;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyToGlobalInitInfo *>(info);

	// Set the error message
	info_ref.error = error;
	info_ref.success = false;
}

void *goose_copy_function_global_init_get_extra_info(goose_copy_function_global_init_info info) {
	if (!info) {
		return nullptr;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyToGlobalInitInfo *>(info);
	return info_ref.bind_data.Cast<goose::CCopyToBindInfo>()
	    .function_info->Cast<goose::CCopyFunctionInfo>()
	    .extra_info;
}

goose_client_context goose_copy_function_global_init_get_client_context(goose_copy_function_global_init_info info) {
	if (!info) {
		return nullptr;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyToGlobalInitInfo *>(info);
	auto wrapper = new goose::CClientContextWrapper(info_ref.context);
	return reinterpret_cast<goose_client_context>(wrapper);
}

void *goose_copy_function_global_init_get_bind_data(goose_copy_function_global_init_info info) {
	if (!info) {
		return nullptr;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyToGlobalInitInfo *>(info);
	auto &bind_info = info_ref.bind_data.Cast<goose::CCopyToBindInfo>();

	return bind_info.bind_data;
}

void goose_copy_function_global_init_set_global_state(goose_copy_function_global_init_info info, void *global_state,
                                                       goose_delete_callback_t destructor) {
	if (!info) {
		return;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyToGlobalInitInfo *>(info);
	info_ref.global_state = global_state;
	info_ref.delete_callback = destructor;
}

const char *goose_copy_function_global_init_get_file_path(goose_copy_function_global_init_info info) {
	if (!info) {
		return nullptr;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyToGlobalInitInfo *>(info);
	return info_ref.file_path.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
// Copy To Local Initialize
//----------------------------------------------------------------------------------------------------------------------
namespace goose {
namespace {

unique_ptr<LocalFunctionData> CCopyToLocalInit(ExecutionContext &context, FunctionData &bind_data) {
	// This isnt exposed to the C-API yet, so we just return empty local function data
	return make_uniq<LocalFunctionData>();
}

} // namespace
} // namespace goose
//----------------------------------------------------------------------------------------------------------------------
// Copy To Sink
//----------------------------------------------------------------------------------------------------------------------
namespace goose {
namespace {

struct CCopyToSinkInfo {
	CCopyToSinkInfo(ClientContext &context, FunctionData &bind_data, GlobalFunctionData &gstate)
	    : context(context), bind_data(bind_data), gstate(gstate) {
	}

	ClientContext &context;
	FunctionData &bind_data;
	GlobalFunctionData &gstate;
	string error;
	bool success = true;
};

void CCopyToSink(ExecutionContext &context, FunctionData &bind_data, GlobalFunctionData &gstate,
                 LocalFunctionData &lstate, DataChunk &input) {
	auto &bind_info = bind_data.Cast<CCopyToBindInfo>();
	auto &function_info = bind_info.function_info->Cast<CCopyFunctionInfo>();

	// Flatten input (we dont support compressed execution yet!)
	// TODO: Dont flatten!
	input.Flatten();

	CCopyToSinkInfo copy_to_sink_info(context.client, bind_data, gstate);

	// Sink is required!
	function_info.sink(reinterpret_cast<goose_copy_function_sink_info>(&copy_to_sink_info),
	                   reinterpret_cast<goose_data_chunk>(&input));

	if (!copy_to_sink_info.success) {
		throw InvalidInputException(copy_to_sink_info.error);
	}
}

} // namespace
} // namespace goose

void goose_copy_function_set_sink(goose_copy_function copy_function, goose_copy_function_sink_t function) {
	if (!copy_function || !function) {
		return;
	}
	auto &copy_function_ref = *reinterpret_cast<goose::CopyFunction *>(copy_function);
	auto &info = copy_function_ref.function_info->Cast<goose::CCopyFunctionInfo>();

	// Set C sink callback
	info.sink = function;
}

void goose_copy_function_sink_set_error(goose_copy_function_sink_info info, const char *error) {
	if (!info || !error) {
		return;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyToSinkInfo *>(info);
	// Set the error message
	info_ref.error = error;
	info_ref.success = false;
}

void *goose_copy_function_sink_get_extra_info(goose_copy_function_sink_info info) {
	if (!info) {
		return nullptr;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyToSinkInfo *>(info);
	return info_ref.bind_data.Cast<goose::CCopyToBindInfo>()
	    .function_info->Cast<goose::CCopyFunctionInfo>()
	    .extra_info;
}

goose_client_context goose_copy_function_sink_get_client_context(goose_copy_function_sink_info info) {
	if (!info) {
		return nullptr;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyToSinkInfo *>(info);
	auto wrapper = new goose::CClientContextWrapper(info_ref.context);
	return reinterpret_cast<goose_client_context>(wrapper);
}

void *goose_copy_function_sink_get_bind_data(goose_copy_function_sink_info info) {
	if (!info) {
		return nullptr;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyToSinkInfo *>(info);
	auto &bind_info = info_ref.bind_data.Cast<goose::CCopyToBindInfo>();

	return bind_info.bind_data;
}

void *goose_copy_function_sink_get_global_state(goose_copy_function_sink_info info) {
	if (!info) {
		return nullptr;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyToSinkInfo *>(info);
	auto &gstate = info_ref.gstate.Cast<goose::CCopyToGlobalState>();

	return gstate.global_state;
}

//----------------------------------------------------------------------------------------------------------------------
// Copy To Combine
//----------------------------------------------------------------------------------------------------------------------
namespace goose {
namespace {

void CCopyToCombine(ExecutionContext &context, FunctionData &bind_data, GlobalFunctionData &gstate,
                    LocalFunctionData &lstate) {
	// Do nothing for now (this isnt exposed to the C-API yet)
}

} // namespace
} // namespace goose

//----------------------------------------------------------------------------------------------------------------------
// Copy To Finalize
//----------------------------------------------------------------------------------------------------------------------
namespace goose {
namespace {

struct CCopyToFinalizeInfo {
	CCopyToFinalizeInfo(ClientContext &context, FunctionData &bind_data, GlobalFunctionData &gstate)
	    : context(context), bind_data(bind_data), gstate(gstate) {
	}

	ClientContext &context;
	FunctionData &bind_data;
	GlobalFunctionData &gstate;

	string error;
	bool success = true;
};

void CCopyToFinalize(ClientContext &context, FunctionData &bind_data, GlobalFunctionData &gstate) {
	auto &bind_info = bind_data.Cast<CCopyToBindInfo>();
	auto &function_info = bind_info.function_info->Cast<CCopyFunctionInfo>();

	// Finalize is optional
	if (function_info.finalize) {
		CCopyToFinalizeInfo copy_to_finalize_info(context, bind_data, gstate);
		function_info.finalize(reinterpret_cast<goose_copy_function_finalize_info>(&copy_to_finalize_info));

		if (!copy_to_finalize_info.success) {
			throw InvalidInputException(copy_to_finalize_info.error);
		}
	}
}

} // namespace
} // namespace goose

void goose_copy_function_set_finalize(goose_copy_function copy_function, goose_copy_function_finalize_t finalize) {
	if (!copy_function || !finalize) {
		return;
	}

	auto &copy_function_ref = *reinterpret_cast<goose::CopyFunction *>(copy_function);
	auto &info = copy_function_ref.function_info->Cast<goose::CCopyFunctionInfo>();

	// Set C finalize callback
	info.finalize = finalize;
}

void goose_copy_function_finalize_set_error(goose_copy_function_finalize_info info, const char *error) {
	if (!info || !error) {
		return;
	}

	auto &info_ref = *reinterpret_cast<goose::CCopyToFinalizeInfo *>(info);
	// Set the error message
	info_ref.error = error;
	info_ref.success = false;
}

void *goose_copy_function_finalize_get_extra_info(goose_copy_function_finalize_info info) {
	if (!info) {
		return nullptr;
	}

	auto &info_ref = *reinterpret_cast<goose::CCopyToFinalizeInfo *>(info);
	return info_ref.bind_data.Cast<goose::CCopyToBindInfo>()
	    .function_info->Cast<goose::CCopyFunctionInfo>()
	    .extra_info;
}

goose_client_context goose_copy_function_finalize_get_client_context(goose_copy_function_finalize_info info) {
	if (!info) {
		return nullptr;
	}
	auto &info_ref = *reinterpret_cast<goose::CCopyToFinalizeInfo *>(info);
	auto wrapper = new goose::CClientContextWrapper(info_ref.context);
	return reinterpret_cast<goose_client_context>(wrapper);
}

void *goose_copy_function_finalize_get_bind_data(goose_copy_function_finalize_info info) {
	if (!info) {
		return nullptr;
	}

	auto &info_ref = *reinterpret_cast<goose::CCopyToFinalizeInfo *>(info);
	auto &bind_info = info_ref.bind_data.Cast<goose::CCopyToBindInfo>();
	return bind_info.bind_data;
}

void *goose_copy_function_finalize_get_global_state(goose_copy_function_finalize_info info) {
	if (!info) {
		return nullptr;
	}

	auto &info_ref = *reinterpret_cast<goose::CCopyToFinalizeInfo *>(info);
	auto &gstate = info_ref.gstate.Cast<goose::CCopyToGlobalState>();
	return gstate.global_state;
}

//----------------------------------------------------------------------------------------------------------------------
// Copy FROM
//----------------------------------------------------------------------------------------------------------------------
namespace goose {
namespace {

unique_ptr<FunctionData> CCopyFromBind(ClientContext &context, CopyFromFunctionBindInput &info,
                                       vector<string> &expected_names, vector<LogicalType> &expected_types) {
	auto &tf_info = info.tf.function_info->Cast<CTableFunctionInfo>();
	auto result = make_uniq<CTableBindData>(tf_info);

	named_parameter_map_t named_parameters;

	// Turn all options into named parameters
	for (auto opt : info.info.options) {
		auto param_it = info.tf.named_parameters.find(opt.first);
		if (param_it == info.tf.named_parameters.end()) {
			// Option not found in the table function's named parameters
			throw BinderException("'%s' is not a supported option for copy function '%s'", opt.first.c_str(),
			                      info.tf.name.c_str());
		}

		// Try to convert a list of values into a single Value, either by extracting or unifying into a list
		Value param_value;
		if (opt.second.empty()) {
			continue;
		}
		if (opt.second.size() == 1) {
			param_value = opt.second[0];
		} else {
			auto first_type = opt.second[0].type();
			auto is_same_type = true;
			for (auto &val : opt.second) {
				if (val.type() != first_type) {
					is_same_type = false;
					break;
				}
			}
			if (is_same_type) {
				param_value = Value::LIST(first_type, opt.second);
			} else {
				throw BinderException("Cannot pass multiple values of different types for copy option '%s'",
				                      opt.first.c_str());
			}
		}

		// Assing the option as a named parameter
		named_parameters[opt.first] = param_value;
	}

	// Also pass file path as a regular parameter
	vector<Value> parameters;
	parameters.push_back(Value(info.info.file_path));

	// Now bind, using the normal table function bind mechanism
	CTableInternalBindInfo bind_info(context, parameters, named_parameters, expected_types, expected_names, *result,
	                                 tf_info);
	tf_info.bind(reinterpret_cast<goose_bind_info>(&bind_info));
	if (!bind_info.success) {
		throw BinderException(bind_info.error);
	}

	return std::move(result);
}

} // namespace
} // namespace goose

void goose_copy_function_set_copy_from_function(goose_copy_function copy_function,
                                                 goose_table_function table_function) {
	auto &copy_function_ref = *reinterpret_cast<goose::CopyFunction *>(copy_function);
	if (!copy_function || !table_function) {
		return;
	}
	auto &tf = *reinterpret_cast<goose::TableFunction *>(table_function);
	auto &tf_info = tf.function_info->Cast<goose::CTableFunctionInfo>();

	if (tf.name.empty()) {
		// Take the name from the copy function if not set
		tf.name = copy_function_ref.name;
	}

	if (!tf_info.bind || !tf_info.init || !tf_info.function) {
		return;
	}
	for (auto it = tf.named_parameters.begin(); it != tf.named_parameters.end(); it++) {
		if (goose::TypeVisitor::Contains(it->second, goose::LogicalTypeId::INVALID)) {
			return;
		}
	}
	for (const auto &argument : tf.arguments) {
		if (goose::TypeVisitor::Contains(argument, goose::LogicalTypeId::INVALID)) {
			return;
		}
	}

	// Set the bind callback to mark this as a "copy from" capable function
	copy_function_ref.copy_from_bind = goose::CCopyFromBind;
	copy_function_ref.copy_from_function = tf;
}

idx_t goose_table_function_bind_get_result_column_count(goose_bind_info bind_info) {
	if (!bind_info) {
		return 0;
	}
	auto &bind_info_ref = *reinterpret_cast<goose::CTableInternalBindInfo *>(bind_info);
	return bind_info_ref.return_types.size();
}

goose_logical_type goose_table_function_bind_get_result_column_type(goose_bind_info bind_info, idx_t col_idx) {
	if (!bind_info) {
		return nullptr;
	}
	auto &bind_info_ref = *reinterpret_cast<goose::CTableInternalBindInfo *>(bind_info);
	if (col_idx >= bind_info_ref.return_types.size()) {
		return nullptr;
	}
	return reinterpret_cast<goose_logical_type>(new goose::LogicalType(bind_info_ref.return_types[col_idx]));
}

const char *goose_table_function_bind_get_result_column_name(goose_bind_info bind_info, idx_t col_idx) {
	if (!bind_info) {
		return nullptr;
	}
	auto &bind_info_ref = *reinterpret_cast<goose::CTableInternalBindInfo *>(bind_info);
	if (col_idx >= bind_info_ref.names.size()) {
		return nullptr;
	}
	return bind_info_ref.names[col_idx].c_str();
}

//----------------------------------------------------------------------------------------------------------------------
// Register
//----------------------------------------------------------------------------------------------------------------------
goose_state goose_register_copy_function(goose_connection connection, goose_copy_function copy_function) {
	if (!connection || !copy_function) {
		return GooseError;
	}

	auto &copy_function_ref = *reinterpret_cast<goose::CopyFunction *>(copy_function);

	// Check that the copy function has a valid name
	if (copy_function_ref.name.empty()) {
		return GooseError;
	}

	auto &info = copy_function_ref.function_info->Cast<goose::CCopyFunctionInfo>();

	auto is_copy_to = false;
	auto is_copy_from = copy_function_ref.copy_from_bind != nullptr;

	if (info.sink) {
		// Set the copy function callbacks
		is_copy_to = true;
		copy_function_ref.copy_to_bind = goose::CCopyToBind;
		copy_function_ref.copy_to_initialize_global = goose::CCopyToGlobalInit;
		copy_function_ref.copy_to_initialize_local = goose::CCopyToLocalInit;
		copy_function_ref.copy_to_sink = goose::CCopyToSink;
		copy_function_ref.copy_to_combine = goose::CCopyToCombine;
		copy_function_ref.copy_to_finalize = goose::CCopyToFinalize;
	}

	if (!is_copy_to && !is_copy_from) {
		// At least one of copy to or copy from must be implemented
		return GooseError;
	}

	auto &conn = *reinterpret_cast<goose::Connection *>(connection);
	try {
		conn.context->RunFunctionInTransaction([&]() {
			auto &catalog = goose::Catalog::GetSystemCatalog(*conn.context);
			goose::CreateCopyFunctionInfo cp_info(copy_function_ref);
			cp_info.on_conflict = goose::OnCreateConflict::ALTER_ON_CONFLICT;
			catalog.CreateCopyFunction(*conn.context, cp_info);
		});
	} catch (...) { // LCOV_EXCL_START
		return GooseError;
	} // LCOV_EXCL_STOP
	return GooseSuccess;
}
