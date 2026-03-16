// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <goose/common/common.h>
#include <goose/common/types/value.h>
#include <goose/common/enums/set_scope.h>

namespace goose {
class ClientContext;
class DatabaseInstance;
struct DBConfig;

const string GetDefaultUserAgent();

enum class SettingScope : uint8_t {
	//! Setting is from the global Setting scope
	GLOBAL,
	//! Setting is from the local Setting scope
	LOCAL,
	//! Setting was not fetched from settings, but it was fetched from a secret instead
	SECRET,
	//! The setting was not found or invalid in some other way
	INVALID
};

struct SettingLookupResult {
public:
	SettingLookupResult() : scope(SettingScope::INVALID) {
	}
	explicit SettingLookupResult(SettingScope scope) : scope(scope) {
		D_ASSERT(scope != SettingScope::INVALID);
	}

public:
	operator bool() { // NOLINT: allow implicit conversion to bool
		return scope != SettingScope::INVALID;
	}

public:
	SettingScope GetScope() {
		D_ASSERT(scope != SettingScope::INVALID);
		return scope;
	}

private:
	SettingScope scope = SettingScope::INVALID;
};

struct SettingCallbackInfo {
	explicit SettingCallbackInfo(ClientContext &context, SetScope scope);
	explicit SettingCallbackInfo(DBConfig &config, optional_ptr<DatabaseInstance> db);

	DBConfig &config;
	optional_ptr<DatabaseInstance> db;
	optional_ptr<ClientContext> context;
	SetScope scope;
};

typedef void (*set_callback_t)(SettingCallbackInfo &info, Value &parameter);
typedef void (*set_global_function_t)(DatabaseInstance *db, DBConfig &config, const Value &parameter);
typedef void (*set_local_function_t)(ClientContext &context, const Value &parameter);
typedef void (*reset_global_function_t)(DatabaseInstance *db, DBConfig &config);
typedef void (*reset_local_function_t)(ClientContext &context);
typedef Value (*get_setting_function_t)(const ClientContext &context);

struct ConfigurationOption {
	const char *name;
	const char *description;
	const char *parameter_type;
	set_global_function_t set_global;
	set_local_function_t set_local;
	reset_global_function_t reset_global;
	reset_local_function_t reset_local;
	get_setting_function_t get_setting;
	SetScope default_scope;
	const char *default_value;
	set_callback_t set_callback;
};

struct ConfigurationAlias {
	const char *alias;
	idx_t option_index;
};

typedef void (*set_option_callback_t)(ClientContext &context, SetScope scope, Value &parameter);

struct ExtensionOption {
	// NOLINTNEXTLINE: work around bug in clang-tidy
	ExtensionOption(string description_p, LogicalType type_p, set_option_callback_t set_function_p,
	                Value default_value_p, SetScope default_scope_p)
	    : description(std::move(description_p)), type(std::move(type_p)), set_function(set_function_p),
	      default_value(std::move(default_value_p)), default_scope(default_scope_p) {
	}

	string description;
	LogicalType type;
	set_option_callback_t set_function;
	Value default_value;
	SetScope default_scope;
};

} // namespace goose
