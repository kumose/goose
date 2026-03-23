//===----------------------------------------------------------------------===//
//                         Goose
//
// goose/storage/external_file_cache_util.cc
//
//
//===----------------------------------------------------------------------===//

#include <goose/storage/external_file_cache_util.h>

#include <goose/common/enum_util.h>
#include <goose/common/enums/cache_validation_mode.h>
#include <goose/common/open_file_info.h>
#include <goose/common/string_util.h>
#include <goose/main/client_context.h>
#include <goose/main/config.h>
#include <goose/main/database.h>
#include <goose/main/settings.h>

namespace goose {

CacheValidationMode ExternalFileCacheUtil::GetCacheValidationMode(const OpenFileInfo &info,
                                                                  optional_ptr<ClientContext> client_context,
                                                                  DatabaseInstance &db) {
	// First check if explicitly set in OpenFileInfo options (per-file override).
	if (info.extended_info != nullptr) {
		const auto &open_options = info.extended_info->options;
		const auto validate_entry = open_options.find("validate_external_file_cache");
		if (validate_entry != open_options.end()) {
			if (validate_entry->second.IsNull()) {
				throw InvalidInputException("Cannot use NULL as argument for validate_external_file_cache");
			}
			const bool validate_cache = BooleanValue::Get(validate_entry->second);
			return validate_cache ? CacheValidationMode::VALIDATE_ALL : CacheValidationMode::NO_VALIDATION;
		}
	}

	// If client context is available, check client-local settings first, then fall back to database config.
	if (client_context) {
		return DBConfig::GetSetting<ValidateExternalFileCacheSetting>(*client_context);
	}

	// No client context, fall back to database config.
	auto &config = DBConfig::GetConfig(db);
	return config.options.validate_external_file_cache;
}

} // namespace goose
