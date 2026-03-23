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
