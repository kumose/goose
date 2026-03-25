#include <goose/main/capi/capi_internal.h>
#include <goose/main/config.h>
#include <goose/common/types/value.h>
#include <goose/main/extension_helper.h>

using goose::DBConfig;
using goose::Value;

// config
goose_state goose_create_config(goose_config *out_config) {
	if (!out_config) {
		return GooseError;
	}
	try {
		*out_config = nullptr;
		auto config = new DBConfig();
		*out_config = reinterpret_cast<goose_config>(config);
		config->SetOptionByName("goose_api", "capi");
	} catch (...) { // LCOV_EXCL_START
		return GooseError;
	} // LCOV_EXCL_STOP
	return GooseSuccess;
}

size_t goose_config_count() {
	return DBConfig::GetOptionCount() + DBConfig::GetAliasCount() +
	       goose::ExtensionHelper::ArraySize(goose::EXTENSION_SETTINGS);
}

goose_state goose_get_config_flag(size_t index, const char **out_name, const char **out_description) {
	auto option = DBConfig::GetOptionByIndex(index);
	if (option) {
		if (out_name) {
			*out_name = option->name;
		}
		if (out_description) {
			*out_description = option->description;
		}
		return GooseSuccess;
	}
	// alias
	index -= DBConfig::GetOptionCount();
	auto alias = DBConfig::GetAliasByIndex(index);
	if (alias) {
		if (out_name) {
			*out_name = alias->alias;
		}
		option = DBConfig::GetOptionByIndex(alias->option_index);
		if (out_description) {
			*out_description = option->description;
		}
		return GooseSuccess;
	}
	index -= DBConfig::GetAliasCount();

	// extension index
	auto entry = goose::ExtensionHelper::GetArrayEntry(goose::EXTENSION_SETTINGS, index);
	if (!entry) {
		return GooseError;
	}
	if (out_name) {
		*out_name = entry->name;
	}
	if (out_description) {
		*out_description = entry->extension;
	}
	return GooseSuccess;
}

goose_state goose_set_config(goose_config config, const char *name, const char *option) {
	if (!config || !name || !option) {
		return GooseError;
	}

	try {
		auto db_config = (DBConfig *)config;
		db_config->SetOptionByName(name, Value(option));
	} catch (...) {
		return GooseError;
	}
	return GooseSuccess;
}

void goose_destroy_config(goose_config *config) {
	if (!config) {
		return;
	}
	if (*config) {
		auto db_config = (DBConfig *)*config;
		delete db_config;
		*config = nullptr;
	}
}
