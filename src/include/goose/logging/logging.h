// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/optional_idx.h>
#include <goose/common/types.h>
#include <goose/common/types-import.h>
#include <goose/common/typedefs.h>

namespace goose {

// Note: `LOG_` prefix is to avoid problems with DEBUG and ERROR macros, these are ToString-ed without the prefix
enum class LogLevel : uint8_t {
	LOG_TRACE = 10,
	LOG_DEBUG = 20,
	LOG_INFO = 30,
	LOG_WARNING = 40,
	LOG_ERROR = 50,
	LOG_FATAL = 60
};

enum class LogContextScope : uint8_t { DATABASE = 10, CONNECTION = 20, THREAD = 30 };

enum class LogMode : uint8_t { LEVEL_ONLY = 0, DISABLE_SELECTED = 1, ENABLE_SELECTED = 2 };

struct LogConfig {
	constexpr static const char *IN_MEMORY_STORAGE_NAME = "memory";
	constexpr static const char *STDOUT_STORAGE_NAME = "stdout";
	constexpr static const char *FILE_STORAGE_NAME = "file";

	constexpr static LogLevel DEFAULT_LOG_LEVEL = LogLevel::LOG_INFO;
	constexpr static const char *DEFAULT_LOG_STORAGE = IN_MEMORY_STORAGE_NAME;

	LogConfig();

	GOOSE_API static LogConfig Create(bool enabled, LogLevel level);
	GOOSE_API static LogConfig CreateFromEnabled(bool enabled, LogLevel level,
	                                              unordered_set<string> &enabled_log_types);
	GOOSE_API static LogConfig CreateFromDisabled(bool enabled, LogLevel level,
	                                               unordered_set<string> &disabled_log_types);

	GOOSE_API bool IsConsistent() const;

	bool enabled;
	LogMode mode;
	LogLevel level;
	string storage;

	unordered_set<string> enabled_log_types;
	unordered_set<string> disabled_log_types;

protected:
	LogConfig(bool enabled, LogLevel level, LogMode mode, optional_ptr<unordered_set<string>> enabled_log_types,
	          optional_ptr<unordered_set<string>> disable_loggers);
};

struct LoggingContext {
	explicit LoggingContext(LogContextScope scope_p) : scope(scope_p) {
	}

	LogContextScope scope;

	optional_idx thread_id;
	optional_idx connection_id;
	optional_idx transaction_id;
	optional_idx query_id;
};

struct RegisteredLoggingContext {
	idx_t context_id;
	LoggingContext context;
};

} // namespace goose
