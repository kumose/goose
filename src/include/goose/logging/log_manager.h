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

#include <goose/logging/logger.h>
#include <goose/logging/log_storage.h>
#include <goose/common/types/timestamp.h>
#include <goose/common/case_insensitive_map.h>

namespace goose {
class LogType;

// Holds global logging state
// - Handles configuration changes
// - Creates Loggers with cached configuration
// - Main sink for logs (either by logging directly into this, or by syncing a pre-cached set of log entries)
// - Holds the log storage
class LogManager {
	friend class ThreadSafeLogger;
	friend class ThreadLocalLogger;
	friend class MutableLogger;

public:
	// Note: two step initialization because Logger needs shared pointer to log manager TODO: can we clean up?
	explicit LogManager(DatabaseInstance &db, LogConfig config = LogConfig());
	~LogManager();
	void Initialize();

	GOOSE_API static LogManager &Get(ClientContext &context);
	unique_ptr<Logger> CreateLogger(LoggingContext context, bool thread_safe = true, bool mutable_settings = false);

	RegisteredLoggingContext RegisterLoggingContext(LoggingContext &context);

	GOOSE_API bool RegisterLogStorage(const string &name, shared_ptr<LogStorage> &storage);

	//! The global logger can be used whe
	GOOSE_API Logger &GlobalLogger();
	GOOSE_API shared_ptr<Logger> GlobalLoggerReference();

	//! Flush everything
	GOOSE_API void Flush();

	//! Get a shared_ptr to the log storage (For example, to scan it)
	GOOSE_API shared_ptr<LogStorage> GetLogStorage();
	GOOSE_API bool CanScan(LoggingTargetTable table);

	GOOSE_API void SetConfig(DatabaseInstance &db, const LogConfig &config);
	GOOSE_API void SetEnableLogging(bool enable);
	GOOSE_API void SetLogMode(LogMode mode);
	GOOSE_API void SetLogLevel(LogLevel level);
	GOOSE_API void SetEnabledLogTypes(optional_ptr<unordered_set<string>> enabled_log_types);
	GOOSE_API void SetDisabledLogTypes(optional_ptr<unordered_set<string>> disabled_log_types);
	GOOSE_API void SetLogStorage(DatabaseInstance &db, const string &storage_name);

	GOOSE_API void UpdateLogStorageConfig(DatabaseInstance &db, case_insensitive_map_t<Value> &config_value);

	GOOSE_API void SetEnableStructuredLoggers(vector<string> &enabled_logger_types);

	GOOSE_API void TruncateLogStorage();

	GOOSE_API LogConfig GetConfig();

	GOOSE_API void RegisterLogType(unique_ptr<LogType> type);
	GOOSE_API optional_ptr<const LogType> LookupLogType(const string &type);
	GOOSE_API void RegisterDefaultLogTypes();

protected:
	RegisteredLoggingContext RegisterLoggingContextInternal(LoggingContext &context);

	// This is to be called by the Loggers only, it does not verify log_level and log_type
	void WriteLogEntry(timestamp_t, const char *log_type, LogLevel log_level, const char *log_message,
	                   const RegisteredLoggingContext &context);
	// This allows efficiently pushing a cached set of log entries into the log manager
	void FlushCachedLogEntries(DataChunk &chunk, const RegisteredLoggingContext &context);

	void SetLogStorageInternal(DatabaseInstance &db, const string &storage_name);

	optional_ptr<const LogType> LookupLogTypeInternal(const string &type);

	void SetConfigInternal(LogConfig config);

	mutex lock;
	LogConfig config;

	shared_ptr<Logger> global_logger;

	shared_ptr<LogStorage> log_storage;

	idx_t next_registered_logging_context_index = 0;

	// Any additional LogStorages registered (by extensions for example)
	case_insensitive_map_t<shared_ptr<LogStorage>> registered_log_storages;
	case_insensitive_map_t<unique_ptr<LogType>> registered_log_types;
};

} // namespace goose
