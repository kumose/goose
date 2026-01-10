#include <goose/main/capi/capi_internal.h>

namespace goose {
namespace {

struct CConfigOption {
	string name;
	LogicalType type;
	Value default_value;
	SetScope default_scope = SetScope::SESSION;
	string description;
};

} // namespace
} // namespace goose

goose_config_option goose_create_config_option() {
	auto coption = new goose::CConfigOption();
	return reinterpret_cast<goose_config_option>(coption);
}

void goose_destroy_config_option(goose_config_option *option) {
	if (!option || !*option) {
		return;
	}
	auto coption = *reinterpret_cast<goose::CConfigOption **>(option);
	delete coption;

	*option = nullptr;
}

void goose_config_option_set_name(goose_config_option option, const char *name) {
	if (!option || !name) {
		return;
	}
	auto coption = reinterpret_cast<goose::CConfigOption *>(option);
	coption->name = name;
}

void goose_config_option_set_type(goose_config_option option, goose_logical_type type) {
	if (!option || !type) {
		return;
	}
	auto coption = reinterpret_cast<goose::CConfigOption *>(option);
	coption->type = *reinterpret_cast<goose::LogicalType *>(type);
}

void goose_config_option_set_default_value(goose_config_option option, goose_value default_value) {
	if (!option || !default_value) {
		return;
	}
	auto coption = reinterpret_cast<goose::CConfigOption *>(option);
	auto cvalue = reinterpret_cast<goose::Value *>(default_value);

	if (coption->type.id() == goose::LogicalTypeId::INVALID) {
		coption->type = cvalue->type();
		coption->default_value = *cvalue;
		return;
	}

	if (coption->type != cvalue->type()) {
		coption->default_value = cvalue->DefaultCastAs(coption->type, false);
		return;
	}

	coption->default_value = *cvalue;
}

void goose_config_option_set_default_scope(goose_config_option option, goose_config_option_scope scope) {
	if (!option) {
		return;
	}
	auto coption = reinterpret_cast<goose::CConfigOption *>(option);
	switch (scope) {
	case GOOSE_CONFIG_OPTION_SCOPE_LOCAL:
		coption->default_scope = goose::SetScope::LOCAL;
		break;
	// Set the option for the current session/connection only.
	case GOOSE_CONFIG_OPTION_SCOPE_SESSION:
		coption->default_scope = goose::SetScope::SESSION;
		break;
	// Set the option globally for all sessions/connections.
	case GOOSE_CONFIG_OPTION_SCOPE_GLOBAL:
		coption->default_scope = goose::SetScope::GLOBAL;
		break;
	default:
		return;
	}
}

void goose_config_option_set_description(goose_config_option option, const char *description) {
	if (!option || !description) {
		return;
	}
	auto coption = reinterpret_cast<goose::CConfigOption *>(option);
	coption->description = description;
}

goose_state goose_register_config_option(goose_connection connection, goose_config_option option) {
	if (!connection || !option) {
		return GooseError;
	}

	auto conn = reinterpret_cast<goose::Connection *>(connection);
	auto coption = reinterpret_cast<goose::CConfigOption *>(option);

	if (coption->name.empty() || coption->type.id() == goose::LogicalTypeId::INVALID) {
		return GooseError;
	}

	// TODO: This is not transactional... but theres no easy way to make it so currently.
	try {
		if (conn->context->db->config.HasExtensionOption(coption->name)) {
			// Option already exists
			return GooseError;
		}
		conn->context->db->config.AddExtensionOption(coption->name, coption->description, coption->type,
		                                             coption->default_value, nullptr, coption->default_scope);
	} catch (...) {
		return GooseError;
	}

	return GooseSuccess;
}

goose_value goose_client_context_get_config_option(goose_client_context context, const char *option_name,
                                                     goose_config_option_scope *out_scope) {
	if (!context || !option_name) {
		return nullptr;
	}

	auto wrapper = reinterpret_cast<goose::CClientContextWrapper *>(context);
	auto &ctx = wrapper->context;

	goose_config_option_scope res_scope = GOOSE_CONFIG_OPTION_SCOPE_INVALID;
	goose::Value *res_value = nullptr;

	goose::Value result;
	switch (ctx.TryGetCurrentSetting(option_name, result).GetScope()) {
	case goose::SettingScope::LOCAL:
		// This is a bit messy, but "session" is presented as LOCAL on the "settings" side of the API.
		res_value = new goose::Value(std::move(result));
		res_scope = GOOSE_CONFIG_OPTION_SCOPE_SESSION;
		break;
	case goose::SettingScope::GLOBAL:
		res_value = new goose::Value(std::move(result));
		res_scope = GOOSE_CONFIG_OPTION_SCOPE_GLOBAL;
		break;
	default:
		res_value = nullptr;
		res_scope = GOOSE_CONFIG_OPTION_SCOPE_INVALID;
		break;
	}

	if (out_scope) {
		*out_scope = res_scope;
	}
	return reinterpret_cast<goose_value>(res_value);
}
