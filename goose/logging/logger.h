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

#include <goose/logging/logging.h>
#include <goose/logging/log_type.h>
#include <goose/common/types.h>
#include <goose/common/string_util.h>

namespace goose {
    class TableDescription;
    class DatabaseInstance;
    class DataChunk;
    class LogManager;
    class ColumnDataCollection;
    class ThreadContext;
    class FileOpener;
    class LogStorage;
    class ExecutionContext;
    struct FileHandle;

    //! Internal
#define GOOSE_LOG_INTERNAL(SOURCE, TYPE, LEVEL, ...)                                                                  \
	{                                                                                                                  \
		auto &logger_ref_ = Logger::Get(SOURCE);                                                                       \
		if (logger_ref_.ShouldLog(TYPE, LEVEL)) {                                                                      \
			logger_ref_.WriteLog(TYPE, LEVEL, __VA_ARGS__);                                                            \
		}                                                                                                              \
	}

    //! Default Loggers
#define GOOSE_LOG_TRACE(SOURCE, ...)                                                                                  \
	GOOSE_LOG_INTERNAL(SOURCE, DefaultLogType::NAME, LogLevel::LOG_TRACE, __VA_ARGS__)
#define GOOSE_LOG_DEBUG(SOURCE, ...)                                                                                  \
	GOOSE_LOG_INTERNAL(SOURCE, DefaultLogType::NAME, LogLevel::LOG_DEBUG, __VA_ARGS__)
#define GOOSE_LOG_INFO(SOURCE, ...) GOOSE_LOG_INTERNAL(SOURCE, DefaultLogType::NAME, LogLevel::LOG_INFO, __VA_ARGS__)
#define GOOSE_LOG_WARNING(SOURCE, ...)                                                                                \
	GOOSE_LOG_INTERNAL(SOURCE, DefaultLogType::NAME, LogLevel::LOG_WARNING, __VA_ARGS__)
#define GOOSE_LOG_ERROR(SOURCE, ...)                                                                                  \
	GOOSE_LOG_INTERNAL(SOURCE, DefaultLogType::NAME, LogLevel::LOG_ERROR, __VA_ARGS__)
#define GOOSE_LOG_FATAL(SOURCE, ...)                                                                                  \
	GOOSE_LOG_INTERNAL(SOURCE, DefaultLogType::NAME, LogLevel::LOG_FATAL, __VA_ARGS__)

    //! LogType based loggers
#define GOOSE_LOG(SOURCE, LOG_TYPE_CLASS, ...)                                                                        \
	GOOSE_LOG_INTERNAL(SOURCE, LOG_TYPE_CLASS::NAME, LOG_TYPE_CLASS::LEVEL,                                           \
	                    LOG_TYPE_CLASS::ConstructLogMessage(__VA_ARGS__))

    //! Main logging interface
    class Logger {
    public:
        GOOSE_API explicit Logger(LogManager &manager) : manager(manager) {
        }

        GOOSE_API virtual ~Logger() = default;

        // Main Logging interface. In most cases the macros above should be used instead of calling these directly
        GOOSE_API virtual bool ShouldLog(const char *log_type, LogLevel log_level) = 0;

        GOOSE_API virtual void WriteLog(const char *log_type, LogLevel log_level, const char *message) = 0;

        // Some more string types for easy logging
        GOOSE_API void WriteLog(const char *log_type, LogLevel log_level, const string &message);

        GOOSE_API void WriteLog(const char *log_type, LogLevel log_level, const string_t &message);

        // Syntactic sugar for formatted strings
        template<typename... ARGS>
        void WriteLog(const char *log_type, LogLevel log_level, const char *format_string, ARGS... params) {
            auto formatted_string = StringUtil::Format(format_string, params...);
            WriteLog(log_type, log_level, formatted_string.c_str());
        }

        GOOSE_API virtual void Flush() = 0;

        // Get the Logger to write log messages to. In decreasing order of preference(!) so the ThreadContext getter is the
        // most preferred way of fetching the logger and the DatabaseInstance getter the least preferred. This has to do
        // both with logging performance and level of detail of logging context that is provided.
        GOOSE_API static Logger &Get(const ThreadContext &thread_context);

        GOOSE_API static Logger &Get(const ExecutionContext &execution_context);

        GOOSE_API static Logger &Get(const ClientContext &client_context);

        GOOSE_API static Logger &Get(const FileOpener &opener);

        GOOSE_API static Logger &Get(const DatabaseInstance &db);

        GOOSE_API static Logger &Get(const shared_ptr<Logger> &logger);

        template<class T>
        static void Flush(T &log_context_source) {
            Get(log_context_source).Flush();
        }

        GOOSE_API virtual bool IsThreadSafe() = 0;

        GOOSE_API virtual bool IsMutable() {
            return false;
        };
        GOOSE_API virtual void UpdateConfig(LogConfig &new_config) {
            throw InternalException("Cannot update the config of this logger!");
        }

        GOOSE_API virtual const LogConfig &GetConfig() const = 0;

    protected:
        LogManager &manager;
    };

    // Thread-safe logger
    class ThreadSafeLogger : public Logger {
    public:
        explicit ThreadSafeLogger(LogConfig &config_p, LoggingContext &context_p, LogManager &manager);

        explicit ThreadSafeLogger(LogConfig &config_p, RegisteredLoggingContext context_p, LogManager &manager);

        // Main Logger API
        bool ShouldLog(const char *log_type, LogLevel log_level) override;

        void WriteLog(const char *log_type, LogLevel log_level, const char *message) override;

        void Flush() override;

        bool IsThreadSafe() override {
            return true;
        }

        const LogConfig &GetConfig() const override {
            return config;
        }

    protected:
        const LogConfig config;
        mutex lock;
        const RegisteredLoggingContext context;
    };

    // Non Thread-safe logger
    // - will cache log entries locally
    class ThreadLocalLogger : public Logger {
    public:
        explicit ThreadLocalLogger(LogConfig &config_p, LoggingContext &context_p, LogManager &manager);

        explicit ThreadLocalLogger(LogConfig &config_p, RegisteredLoggingContext context_p, LogManager &manager);

        // Main Logger API
        bool ShouldLog(const char *log_type, LogLevel log_level) override;

        void WriteLog(const char *log_type, LogLevel log_level, const char *message) override;

        void Flush() override;

        bool IsThreadSafe() override {
            return false;
        }

        const LogConfig &GetConfig() const override {
            return config;
        }

    protected:
        const LogConfig config;
        const RegisteredLoggingContext context;
    };

    // Thread-safe Logger with mutable log settings
    class MutableLogger : public Logger {
    public:
        explicit MutableLogger(LogConfig &config_p, LoggingContext &context_p, LogManager &manager);

        explicit MutableLogger(LogConfig &config_p, RegisteredLoggingContext context_p, LogManager &manager);

        // Main Logger API
        bool ShouldLog(const char *log_type, LogLevel log_level) override;

        void WriteLog(const char *log_type, LogLevel log_level, const char *message) override;

        void Flush() override;

        bool IsThreadSafe() override {
            return true;
        }

        bool IsMutable() override {
            return true;
        }

        const LogConfig &GetConfig() const override {
            return config;
        }

        void UpdateConfig(LogConfig &new_config) override;

    protected:
        // Atomics for lock-free log setting checks
        atomic<bool> enabled;
        atomic<LogMode> mode;
        atomic<LogLevel> level;

        mutex lock;
        LogConfig config;
        const RegisteredLoggingContext context;
    };

    // For when logging is disabled: NOPs everything
    class NopLogger : public Logger {
    public:
        explicit NopLogger(LogManager &manager) : Logger(manager) {
        }

        bool ShouldLog(const char *log_type, LogLevel log_level) override {
            return false;
        }

        void WriteLog(const char *log_type, LogLevel log_level, const char *message) override {
        };

        void Flush() override {
        }

        bool IsThreadSafe() override {
            return true;
        }

        const LogConfig &GetConfig() const override {
            throw InternalException("Called GetConfig on NopLogger");
        }
    };
} // namespace goose
