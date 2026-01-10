#include <goose/main/capi/capi_internal.h>

using goose::CClientArrowOptionsWrapper;
using goose::CClientContextWrapper;
using goose::Connection;
using goose::DatabaseWrapper;
using goose::DBConfig;
using goose::DBInstanceCacheWrapper;
using goose::Goose;
using goose::ErrorData;

goose_instance_cache goose_create_instance_cache() {
	auto wrapper = new DBInstanceCacheWrapper();
	wrapper->instance_cache = goose::make_uniq<goose::DBInstanceCache>();
	return reinterpret_cast<goose_instance_cache>(wrapper);
}

void goose_destroy_instance_cache(goose_instance_cache *instance_cache) {
	if (instance_cache && *instance_cache) {
		auto wrapper = reinterpret_cast<DBInstanceCacheWrapper *>(*instance_cache);
		delete wrapper;
		*instance_cache = nullptr;
	}
}

goose_state goose_open_internal(DBInstanceCacheWrapper *cache, const char *path, goose_database *out,
                                  goose_config config, char **out_error) {
	auto wrapper = new DatabaseWrapper();
	try {
		DBConfig default_config;
		default_config.SetOptionByName("goose_api", "capi");

		DBConfig *db_config = &default_config;
		DBConfig *user_config = reinterpret_cast<DBConfig *>(config);
		if (user_config) {
			db_config = user_config;
		}

		if (cache) {
			goose::string path_str;
			if (path) {
				path_str = path;
			}
			wrapper->database = cache->instance_cache->GetOrCreateInstance(path_str, *db_config);
		} else {
			wrapper->database = goose::make_shared_ptr<Goose>(path, db_config);
		}

	} catch (std::exception &ex) {
		if (out_error) {
			ErrorData parsed_error(ex);
			*out_error = strdup(parsed_error.Message().c_str());
		}
		delete wrapper;
		return GooseError;

	} catch (...) { // LCOV_EXCL_START
		if (out_error) {
			*out_error = strdup("Unknown error");
		}
		delete wrapper;
		return GooseError;
	} // LCOV_EXCL_STOP

	*out = reinterpret_cast<goose_database>(wrapper);
	return GooseSuccess;
}

goose_state goose_get_or_create_from_cache(goose_instance_cache instance_cache, const char *path,
                                             goose_database *out_database, goose_config config, char **out_error) {
	if (!instance_cache) {
		if (out_error) {
			*out_error = strdup("instance cache cannot be nullptr");
		}
		return GooseError;
	}
	auto cache = reinterpret_cast<DBInstanceCacheWrapper *>(instance_cache);
	return goose_open_internal(cache, path, out_database, config, out_error);
}

goose_state goose_open_ext(const char *path, goose_database *out, goose_config config, char **error) {
	return goose_open_internal(nullptr, path, out, config, error);
}

goose_state goose_open(const char *path, goose_database *out) {
	return goose_open_ext(path, out, nullptr, nullptr);
}

void goose_close(goose_database *database) {
	if (database && *database) {
		auto wrapper = reinterpret_cast<DatabaseWrapper *>(*database);
		delete wrapper;
		*database = nullptr;
	}
}

goose_state goose_connect(goose_database database, goose_connection *out) {
	if (!database || !out) {
		return GooseError;
	}

	auto wrapper = reinterpret_cast<DatabaseWrapper *>(database);
	Connection *connection;
	try {
		connection = new Connection(*wrapper->database);
	} catch (...) { // LCOV_EXCL_START
		return GooseError;
	} // LCOV_EXCL_STOP

	*out = reinterpret_cast<goose_connection>(connection);
	return GooseSuccess;
}

void goose_interrupt(goose_connection connection) {
	if (!connection) {
		return;
	}
	Connection *conn = reinterpret_cast<Connection *>(connection);
	conn->Interrupt();
}

goose_query_progress_type goose_query_progress(goose_connection connection) {
	goose_query_progress_type query_progress_type;
	query_progress_type.percentage = -1;
	query_progress_type.total_rows_to_process = 0;
	query_progress_type.rows_processed = 0;
	if (!connection) {
		return query_progress_type;
	}
	Connection *conn = reinterpret_cast<Connection *>(connection);
	auto query_progress = conn->context->GetQueryProgress();
	query_progress_type.total_rows_to_process = query_progress.GetTotalRowsToProcess();
	query_progress_type.rows_processed = query_progress.GetRowsProcesseed();
	query_progress_type.percentage = query_progress.GetPercentage();
	return query_progress_type;
}

void goose_disconnect(goose_connection *connection) {
	if (connection && *connection) {
		Connection *conn = reinterpret_cast<Connection *>(*connection);
		delete conn;
		*connection = nullptr;
	}
}

void goose_connection_get_client_context(goose_connection connection, goose_client_context *out_context) {
	if (!connection || !out_context) {
		return;
	}
	Connection *conn = reinterpret_cast<Connection *>(connection);
	try {
		auto wrapper = new CClientContextWrapper(*conn->context);
		*out_context = reinterpret_cast<goose_client_context>(wrapper);
	} catch (...) {
		*out_context = nullptr;
	}
}

void goose_connection_get_arrow_options(goose_connection connection, goose_arrow_options *out_arrow_options) {
	if (!connection || !out_arrow_options) {
		return;
	}
	Connection *conn = reinterpret_cast<Connection *>(connection);
	try {
		auto client_properties = conn->context->GetClientProperties();
		auto wrapper = new CClientArrowOptionsWrapper(client_properties);
		*out_arrow_options = reinterpret_cast<goose_arrow_options>(wrapper);
	} catch (...) {
		*out_arrow_options = nullptr;
	}
}

idx_t goose_client_context_get_connection_id(goose_client_context context) {
	auto wrapper = reinterpret_cast<CClientContextWrapper *>(context);
	return wrapper->context.GetConnectionId();
}

void goose_destroy_client_context(goose_client_context *context) {
	if (context && *context) {
		auto wrapper = reinterpret_cast<CClientContextWrapper *>(*context);
		delete wrapper;
		*context = nullptr;
	}
}

void goose_destroy_arrow_options(goose_arrow_options *arrow_options) {
	if (arrow_options && *arrow_options) {
		auto wrapper = reinterpret_cast<CClientArrowOptionsWrapper *>(*arrow_options);
		delete wrapper;
		*arrow_options = nullptr;
	}
}

goose_state goose_query(goose_connection connection, const char *query, goose_result *out) {
	Connection *conn = reinterpret_cast<Connection *>(connection);
	try {
		auto result = conn->Query(query);
		return GooseTranslateResult(std::move(result), out);
	} catch (...) {
		return GooseError;
	}
}

const char *goose_library_version() {
	return Goose::LibraryVersion();
}

goose_value goose_get_table_names(goose_connection connection, const char *query, bool qualified) {
	Connection *conn = reinterpret_cast<Connection *>(connection);
	try {
		auto table_names = conn->GetTableNames(query, qualified);

		auto count = table_names.size();
		auto ptr = malloc(count * sizeof(goose_value));
		if (!ptr) {
			return nullptr;
		}
		auto list_values = reinterpret_cast<goose_value *>(ptr);

		try {
			idx_t name_ix = 0;
			for (const auto &name : table_names) {
				list_values[name_ix] = goose_create_varchar(name.c_str());
				name_ix++;
			}

			auto varchar_type = goose_create_logical_type(GOOSE_TYPE_VARCHAR);
			auto list_value = goose_create_list_value(varchar_type, list_values, count);

			for (idx_t i = 0; i < count; i++) {
				goose_destroy_value(&list_values[i]);
			}
			goose_free(ptr);
			goose_destroy_logical_type(&varchar_type);

			return list_value;
		} catch (...) {
			goose_free(ptr);
			return nullptr;
		}
	} catch (...) {
		return nullptr;
	}
}
