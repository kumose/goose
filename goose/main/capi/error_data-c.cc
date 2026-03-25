#include <goose/main/capi/capi_internal.h>

using goose::ErrorData;
using goose::ErrorDataWrapper;

goose_error_data goose_create_error_data(goose_error_type type, const char *message) {
	auto wrapper = new ErrorDataWrapper();
	wrapper->error_data = ErrorData(goose::ErrorTypeFromC(type), message);
	return reinterpret_cast<goose_error_data>(wrapper);
}

void goose_destroy_error_data(goose_error_data *error_data) {
	if (!error_data || !*error_data) {
		return;
	}
	auto wrapper = reinterpret_cast<ErrorDataWrapper *>(*error_data);
	delete wrapper;
	*error_data = nullptr;
}

goose_error_type goose_error_data_error_type(goose_error_data error_data) {
	if (!error_data) {
		return GOOSE_ERROR_INVALID_TYPE;
	}

	auto *wrapper = reinterpret_cast<ErrorDataWrapper *>(error_data);
	return goose::ErrorTypeToC(wrapper->error_data.Type());
}

const char *goose_error_data_message(goose_error_data error_data) {
	if (!error_data) {
		return nullptr;
	}

	auto *wrapper = reinterpret_cast<ErrorDataWrapper *>(error_data);
	return wrapper->error_data.RawMessage().c_str();
}

bool goose_error_data_has_error(goose_error_data error_data) {
	if (!error_data) {
		return false;
	}

	auto *wrapper = reinterpret_cast<ErrorDataWrapper *>(error_data);
	return wrapper->error_data.HasError();
}
