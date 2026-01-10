#include <goose/main/capi/capi_internal.h>
#include <goose/logging/log_storage.h>

namespace goose {

class CallbackLogStorage : public LogStorage {
public:
	CallbackLogStorage(const string &name, goose_logger_write_log_entry_t write_log_entry_fun, void *extra_data,
	                   goose_delete_callback_t delete_callback)
	    : name(name), write_log_entry_fun(write_log_entry_fun), extra_data(extra_data),
	      delete_callback(delete_callback) {
	}

	~CallbackLogStorage() override {
		if (!extra_data || !delete_callback) {
			return;
		}
		delete_callback(extra_data);
	}

	void WriteLogEntry(timestamp_t timestamp, LogLevel level, const string &log_type, const string &log_message,
	                   const RegisteredLoggingContext &context) override {
		if (write_log_entry_fun == nullptr) {
			return;
		}
		auto c_timestamp = reinterpret_cast<goose_timestamp *>(&timestamp);
		write_log_entry_fun(extra_data, c_timestamp, EnumUtil::ToChars(level), log_type.c_str(), log_message.c_str());
	};

	void WriteLogEntries(DataChunk &chunk, const RegisteredLoggingContext &context) override {};

	void Flush(LoggingTargetTable table) override {};

	void FlushAll() override {};

	bool IsEnabled(LoggingTargetTable table) override {
		return true;
	}

	const string GetStorageName() override {
		return name;
	}

private:
	const string name;
	goose_logger_write_log_entry_t write_log_entry_fun;
	void *extra_data;
	goose_delete_callback_t delete_callback;
};

struct LogStorageWrapper {
	string name;
	goose_logger_write_log_entry_t write_log_entry = nullptr;
	void *extra_data = nullptr;
	goose_delete_callback_t delete_callback = nullptr;
};

} // namespace goose

using goose::DatabaseWrapper;
using goose::LogStorageWrapper;

goose_log_storage goose_create_log_storage() {
	auto log_storage_wrapper = new LogStorageWrapper();
	return reinterpret_cast<goose_log_storage>(log_storage_wrapper);
}

void goose_destroy_log_storage(goose_log_storage *log_storage) {
	if (log_storage && *log_storage) {
		auto log_storage_wrapper = reinterpret_cast<LogStorageWrapper *>(*log_storage);
		if (log_storage_wrapper->extra_data && log_storage_wrapper->delete_callback) {
			log_storage_wrapper->delete_callback(log_storage_wrapper->extra_data);
		}
		delete log_storage_wrapper;
		*log_storage = nullptr;
	}
}

void goose_log_storage_set_write_log_entry(goose_log_storage log_storage, goose_logger_write_log_entry_t function) {
	if (!log_storage || !function) {
		return;
	}

	auto log_storage_wrapper = reinterpret_cast<LogStorageWrapper *>(log_storage);
	log_storage_wrapper->write_log_entry = function;
}

void goose_log_storage_set_extra_data(goose_log_storage log_storage, void *extra_data,
                                       goose_delete_callback_t delete_callback) {
	if (!log_storage) {
		return;
	}

	auto log_storage_wrapper = reinterpret_cast<LogStorageWrapper *>(log_storage);
	log_storage_wrapper->extra_data = extra_data;
	log_storage_wrapper->delete_callback = delete_callback;
}

void goose_log_storage_set_name(goose_log_storage log_storage, const char *name) {
	if (!log_storage || !name) {
		return;
	}
	auto log_storage_wrapper = reinterpret_cast<LogStorageWrapper *>(log_storage);
	log_storage_wrapper->name = name;
}

goose_state goose_register_log_storage(goose_database database, goose_log_storage log_storage) {
	if (!database || !log_storage) {
		return GooseError;
	}

	const auto db_wrapper = reinterpret_cast<DatabaseWrapper *>(database);
	auto log_storage_wrapper = reinterpret_cast<LogStorageWrapper *>(log_storage);
	if (log_storage_wrapper->name.empty() || log_storage_wrapper->write_log_entry == nullptr) {
		return GooseError;
	}

	const auto &db = *db_wrapper->database;
	auto shared_storage_ptr = goose::make_shared_ptr<goose::CallbackLogStorage>(
	    log_storage_wrapper->name, log_storage_wrapper->write_log_entry, log_storage_wrapper->extra_data,
	    log_storage_wrapper->delete_callback);
	goose::shared_ptr<goose::LogStorage> storage_ptr = shared_storage_ptr;

	const auto success = db.instance->GetLogManager().RegisterLogStorage(log_storage_wrapper->name, storage_ptr);
	if (!success) {
		return GooseError;
	}

	// Avoid leaking memory in case of registration failure because
	// we transfer ownership when creating the shared pointer.
	log_storage_wrapper->extra_data = nullptr;
	log_storage_wrapper->delete_callback = nullptr;
	return GooseSuccess;
}
