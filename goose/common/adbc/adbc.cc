#include <goose/common/adbc/adbc-goose.h>
#include <goose/common/adbc/adbc-init.h>

#include <goose/common/string.h>
#include <goose/common/string_util.h>

#include <goose/goose-c.h>
#include <goose/common/arrow/arrow_wrapper.h>
#include <goose/common/arrow/nanoarrow/nanoarrow-goose.h>

#include <goose/main/connection.h>
#include <goose/common/adbc/options.h>
#include <goose/common/adbc/single_batch_array_stream.h>
#include <goose/function/table/arrow.h>
#include <goose/common/adbc/wrappers.h>
#include <stdlib.h>
static void ReleaseError(struct AdbcError *error);

#include <string.h>

#include <goose/main/prepared_statement_data.h>

#include <goose/parser/keyword_helper.h>

// We must leak the symbols of the init function
AdbcStatusCode goose_adbc_init(int version, void *driver, struct AdbcError *error) {
	if (!driver) {
		return ADBC_STATUS_INVALID_ARGUMENT;
	}

	// Check that the version is supported (1.0.0 or 1.1.0)
	if (version != ADBC_VERSION_1_0_0 && version != ADBC_VERSION_1_1_0) {
		return ADBC_STATUS_NOT_IMPLEMENTED;
	}

	auto adbc_driver = static_cast<AdbcDriver *>(driver);

	// Initialize all 1.0.0 function pointers
	adbc_driver->DatabaseNew = goose_adbc::DatabaseNew;
	adbc_driver->DatabaseSetOption = goose_adbc::DatabaseSetOption;
	adbc_driver->DatabaseInit = goose_adbc::DatabaseInit;
	adbc_driver->DatabaseRelease = goose_adbc::DatabaseRelease;
	adbc_driver->ConnectionNew = goose_adbc::ConnectionNew;
	adbc_driver->ConnectionSetOption = goose_adbc::ConnectionSetOption;
	adbc_driver->ConnectionInit = goose_adbc::ConnectionInit;
	adbc_driver->ConnectionRelease = goose_adbc::ConnectionRelease;
	adbc_driver->ConnectionGetTableTypes = goose_adbc::ConnectionGetTableTypes;
	adbc_driver->StatementNew = goose_adbc::StatementNew;
	adbc_driver->StatementRelease = goose_adbc::StatementRelease;
	adbc_driver->StatementBind = goose_adbc::StatementBind;
	adbc_driver->StatementBindStream = goose_adbc::StatementBindStream;
	adbc_driver->StatementExecuteQuery = goose_adbc::StatementExecuteQuery;
	adbc_driver->StatementPrepare = goose_adbc::StatementPrepare;
	adbc_driver->StatementSetOption = goose_adbc::StatementSetOption;
	adbc_driver->StatementSetSqlQuery = goose_adbc::StatementSetSqlQuery;
	adbc_driver->ConnectionGetObjects = goose_adbc::ConnectionGetObjects;
	adbc_driver->ConnectionCommit = goose_adbc::ConnectionCommit;
	adbc_driver->ConnectionRollback = goose_adbc::ConnectionRollback;
	adbc_driver->ConnectionReadPartition = goose_adbc::ConnectionReadPartition;
	adbc_driver->StatementExecutePartitions = goose_adbc::StatementExecutePartitions;
	adbc_driver->ConnectionGetInfo = goose_adbc::ConnectionGetInfo;
	adbc_driver->StatementGetParameterSchema = goose_adbc::StatementGetParameterSchema;
	adbc_driver->ConnectionGetTableSchema = goose_adbc::ConnectionGetTableSchema;

	// Initialize 1.1.0 function pointers if version >= 1.1.0
	if (version >= ADBC_VERSION_1_1_0) {
		// TODO: ADBC 1.1.0 adds support for these functions
		adbc_driver->ErrorGetDetailCount = nullptr;
		adbc_driver->ErrorGetDetail = nullptr;
		adbc_driver->ErrorFromArrayStream = nullptr;

		adbc_driver->DatabaseGetOption = nullptr;
		adbc_driver->DatabaseGetOptionBytes = nullptr;
		adbc_driver->DatabaseGetOptionDouble = nullptr;
		adbc_driver->DatabaseGetOptionInt = nullptr;
		adbc_driver->DatabaseSetOptionBytes = nullptr;
		adbc_driver->DatabaseSetOptionInt = nullptr;
		adbc_driver->DatabaseSetOptionDouble = nullptr;

		adbc_driver->ConnectionCancel = nullptr;
		adbc_driver->ConnectionGetOption = nullptr;
		adbc_driver->ConnectionGetOptionBytes = nullptr;
		adbc_driver->ConnectionGetOptionDouble = nullptr;
		adbc_driver->ConnectionGetOptionInt = nullptr;
		adbc_driver->ConnectionGetStatistics = nullptr;
		adbc_driver->ConnectionGetStatisticNames = nullptr;
		adbc_driver->ConnectionSetOptionBytes = nullptr;
		adbc_driver->ConnectionSetOptionInt = nullptr;
		adbc_driver->ConnectionSetOptionDouble = nullptr;

		adbc_driver->StatementCancel = nullptr;
		adbc_driver->StatementExecuteSchema = nullptr;
		adbc_driver->StatementGetOption = nullptr;
		adbc_driver->StatementGetOptionBytes = nullptr;
		adbc_driver->StatementGetOptionDouble = nullptr;
		adbc_driver->StatementGetOptionInt = nullptr;
		adbc_driver->StatementSetOptionBytes = nullptr;
		adbc_driver->StatementSetOptionDouble = nullptr;
		adbc_driver->StatementSetOptionInt = nullptr;
	}

	return ADBC_STATUS_OK;
}

namespace goose_adbc {

// ADBC 1.1.0: Added REPLACE and CREATE_APPEND modes
enum class IngestionMode { CREATE = 0, APPEND = 1, REPLACE = 2, CREATE_APPEND = 3 };

struct GooseAdbcStatementWrapper {
	goose_connection connection;
	goose_prepared_statement statement;
	char *ingestion_table_name;
	char *db_schema;
	ArrowArrayStream ingestion_stream;
	IngestionMode ingestion_mode = IngestionMode::CREATE;
	bool temporary_table = false;
	uint64_t plan_length;
};

struct GooseAdbcStreamWrapper {
	goose_result result;
};

static AdbcStatusCode QueryInternal(struct AdbcConnection *connection, struct ArrowArrayStream *out, const char *query,
                                    struct AdbcError *error) {
	AdbcStatement statement;

	auto status = StatementNew(connection, &statement, error);
	if (status != ADBC_STATUS_OK) {
		StatementRelease(&statement, error);
		SetError(error, "unable to initialize statement");
		return status;
	}
	status = StatementSetSqlQuery(&statement, query, error);
	if (status != ADBC_STATUS_OK) {
		StatementRelease(&statement, error);
		SetError(error, "unable to initialize statement");
		return status;
	}
	status = StatementExecuteQuery(&statement, out, nullptr, error);
	if (status != ADBC_STATUS_OK) {
		StatementRelease(&statement, error);
		SetError(error, "unable to initialize statement");
		return status;
	}
	StatementRelease(&statement, error);
	return ADBC_STATUS_OK;
}

struct GooseAdbcDatabaseWrapper {
	//! The Goose Database Configuration
	goose_config config = nullptr;
	//! The Goose Database
	goose_database database = nullptr;
	//! Path of Disk-Based Database or :memory: database (ADBC "path" option)
	std::string path;
	//! Derived path from ADBC "uri" option (after minimal normalization)
	std::string uri_path;
	bool uri_set = false;
};

void InitializeADBCError(AdbcError *error) {
	if (!error) {
		return;
	}
	// Avoid leaking any Goose-owned error message.
	// Only call Goose's own release callback.
	if (error->message && error->release == ::ReleaseError) {
		error->release(error);
	}
	error->message = nullptr;
	// Don't set to nullptr, as that indicates that it's invalid
	// Use Goose's release callback even for an "empty" error.
	error->release = ::ReleaseError;
	std::memset(error->sqlstate, '\0', sizeof(error->sqlstate));
	error->vendor_code = -1;
}

AdbcStatusCode CheckResult(const goose_state &res, AdbcError *error, const char *error_msg) {
	if (!error) {
		// Error should be a non-null pointer
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (res != GooseSuccess) {
		SetError(error, error_msg);
		return ADBC_STATUS_INTERNAL;
	}
	return ADBC_STATUS_OK;
}

AdbcStatusCode DatabaseNew(struct AdbcDatabase *database, struct AdbcError *error) {
	if (!database) {
		SetError(error, "Missing database object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	database->private_data = nullptr;
	// you can't malloc a struct with a non-trivial C++ constructor
	// and std::string has a non-trivial constructor. so we need
	// to use new and delete rather than malloc and free.
	auto wrapper = new (std::nothrow) GooseAdbcDatabaseWrapper;
	if (!wrapper) {
		SetError(error, "Allocation error");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	database->private_data = wrapper;
	auto res = goose_create_config(&wrapper->config);
	return CheckResult(res, error, "Failed to allocate");
}

AdbcStatusCode DatabaseSetOption(struct AdbcDatabase *database, const char *key, const char *value,
                                 struct AdbcError *error) {
	if (!database) {
		SetError(error, "Missing database object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!key) {
		SetError(error, "Missing key");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}

	auto wrapper = static_cast<GooseAdbcDatabaseWrapper *>(database->private_data);
	if (strcmp(key, "path") == 0) {
		wrapper->path = value;
		return ADBC_STATUS_OK;
	}
	if (strcmp(key, "uri") == 0) {
		if (strncmp(value, "file:", 5) != 0) {
			wrapper->uri_path = value;
			wrapper->uri_set = true;
			return ADBC_STATUS_OK;
		}
		std::string file_path(value + 5);
		auto suffix_pos = file_path.find_first_of("?#");
		if (suffix_pos != std::string::npos) {
			file_path.erase(suffix_pos);
		}
		if (goose::StringUtil::StartsWith(file_path, "//")) {
			auto path_start = file_path.find('/', 2);
			std::string authority =
			    (path_start == std::string::npos) ? file_path.substr(2) : file_path.substr(2, path_start - 2);
			auto authority_lc = goose::StringUtil::Lower(authority);
			if (path_start == std::string::npos) {
				// Accept file://foo as a relative path for compatibility (e.g., arrow-adbc recipe driver example).
				file_path = (authority_lc.empty() || authority_lc == "localhost") ? std::string() : authority;
			} else {
				if (!authority_lc.empty() && authority_lc != "localhost") {
					SetError(error, "file: URI with a non-empty authority is not supported");
					return ADBC_STATUS_INVALID_ARGUMENT;
				}
				file_path = file_path.substr(path_start);
			}
		}
		wrapper->uri_path = std::move(file_path);
		wrapper->uri_set = true;
		return ADBC_STATUS_OK;
	}
	auto res = goose_set_config(wrapper->config, key, value);

	return CheckResult(res, error, "Failed to set configuration option");
}

AdbcStatusCode DatabaseInit(struct AdbcDatabase *database, struct AdbcError *error) {
	if (!error) {
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!database) {
		SetError(error, "ADBC Database has an invalid pointer");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	char *errormsg = nullptr;
	// TODO can we set the database path via option, too? Does not look like it...
	auto wrapper = static_cast<GooseAdbcDatabaseWrapper *>(database->private_data);
	const auto &db_path = wrapper->uri_set ? wrapper->uri_path : wrapper->path;
	auto res = goose_open_ext(db_path.c_str(), &wrapper->database, wrapper->config, &errormsg);
	auto adbc_result = CheckResult(res, error, errormsg);
	if (errormsg) {
		free(errormsg);
	}
	return adbc_result;
}

AdbcStatusCode DatabaseRelease(struct AdbcDatabase *database, struct AdbcError *error) {
	if (database && database->private_data) {
		auto wrapper = static_cast<GooseAdbcDatabaseWrapper *>(database->private_data);

		goose_close(&wrapper->database);
		goose_destroy_config(&wrapper->config);
		delete wrapper;
		database->private_data = nullptr;
	}
	return ADBC_STATUS_OK;
}

AdbcStatusCode ConnectionGetTableSchema(struct AdbcConnection *connection, const char *catalog, const char *db_schema,
                                        const char *table_name, struct ArrowSchema *schema, struct AdbcError *error) {
	if (!connection) {
		SetError(error, "Connection is not set");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (db_schema == nullptr || strlen(db_schema) == 0) {
		// if schema is not set, we use the default schema
		db_schema = "main";
	}
	if (table_name == nullptr) {
		SetError(error, "AdbcConnectionGetTableSchema: must provide table_name");
		return ADBC_STATUS_INVALID_ARGUMENT;
	} else if (strlen(table_name) == 0) {
		SetError(error, "AdbcConnectionGetTableSchema: must provide table_name");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	ArrowArrayStream arrow_stream;

	std::string query = "SELECT * FROM ";
	if (catalog != nullptr && strlen(catalog) > 0) {
		query += goose::KeywordHelper::WriteOptionallyQuoted(catalog) + ".";
	}
	query += goose::KeywordHelper::WriteOptionallyQuoted(db_schema) + ".";
	query += goose::KeywordHelper::WriteOptionallyQuoted(table_name) + " LIMIT 0;";

	auto success = QueryInternal(connection, &arrow_stream, query.c_str(), error);
	if (success != ADBC_STATUS_OK) {
		return success;
	}
	arrow_stream.get_schema(&arrow_stream, schema);
	arrow_stream.release(&arrow_stream);
	return ADBC_STATUS_OK;
}

AdbcStatusCode ConnectionNew(struct AdbcConnection *connection, struct AdbcError *error) {
	if (!connection) {
		SetError(error, "Missing connection object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}

	auto connection_wrapper = new goose::GooseAdbcConnectionWrapper();
	connection_wrapper->connection = nullptr;
	connection->private_data = connection_wrapper;
	return ADBC_STATUS_OK;
}

AdbcStatusCode ExecuteQuery(goose::Connection *conn, const char *query, struct AdbcError *error) {
	auto res = conn->Query(query);
	if (res->HasError()) {
		auto error_message = "Failed to execute query \"" + std::string(query) + "\": " + res->GetError();
		SetError(error, error_message);
		return ADBC_STATUS_INTERNAL;
	}
	return ADBC_STATUS_OK;
}

AdbcStatusCode InternalSetOption(goose::Connection &conn, std::unordered_map<std::string, std::string> &options,
                                 struct AdbcError *error) {
	// If we got here, the options have already been validated and are acceptable
	for (auto &option : options) {
		if (strcmp(option.first.c_str(), ADBC_CONNECTION_OPTION_AUTOCOMMIT) == 0) {
			if (strcmp(option.second.c_str(), ADBC_OPTION_VALUE_ENABLED) == 0) {
				if (conn.HasActiveTransaction()) {
					AdbcStatusCode status = ExecuteQuery(&conn, "COMMIT", error);
					if (status != ADBC_STATUS_OK) {
						options.clear();
						return status;
					}
				}
			} else if (strcmp(option.second.c_str(), ADBC_OPTION_VALUE_DISABLED) == 0) {
				if (!conn.HasActiveTransaction()) {
					AdbcStatusCode status = ExecuteQuery(&conn, "START TRANSACTION", error);
					if (status != ADBC_STATUS_OK) {
						options.clear();
						return status;
					}
				}
			}
		}
	}
	options.clear();
	return ADBC_STATUS_OK;
}
AdbcStatusCode ConnectionSetOption(struct AdbcConnection *connection, const char *key, const char *value,
                                   struct AdbcError *error) {
	if (!connection) {
		SetError(error, "Connection is not set");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	std::string key_string = std::string(key);
	std::string key_value = std::string(value);
	auto conn_wrapper = static_cast<goose::GooseAdbcConnectionWrapper *>(connection->private_data);
	if (strcmp(key, ADBC_CONNECTION_OPTION_AUTOCOMMIT) == 0) {
		if (strcmp(value, ADBC_OPTION_VALUE_ENABLED) == 0) {
			conn_wrapper->options[key_string] = key_value;
		} else if (strcmp(value, ADBC_OPTION_VALUE_DISABLED) == 0) {
			conn_wrapper->options[key_string] = key_value;
		} else {
			auto error_message = "Invalid connection option value " + std::string(key) + "=" + std::string(value);
			SetError(error, error_message);
			return ADBC_STATUS_INVALID_ARGUMENT;
		}
	} else {
		// This is an unknown option to the Goose driver
		auto error_message =
		    "Unknown connection option " + std::string(key) + "=" + (value ? std::string(value) : "(NULL)");
		SetError(error, error_message);
		return ADBC_STATUS_NOT_IMPLEMENTED;
	}
	if (!conn_wrapper->connection) {
		// If the connection has not yet been initialized, we just return here.
		return ADBC_STATUS_OK;
	}
	auto conn = reinterpret_cast<goose::Connection *>(conn_wrapper->connection);
	return InternalSetOption(*conn, conn_wrapper->options, error);
}

AdbcStatusCode ConnectionReadPartition(struct AdbcConnection *connection, const uint8_t *serialized_partition,
                                       size_t serialized_length, struct ArrowArrayStream *out,
                                       struct AdbcError *error) {
	SetError(error, "Read Partitions are not supported in Goose");
	return ADBC_STATUS_NOT_IMPLEMENTED;
}

AdbcStatusCode StatementExecutePartitions(struct AdbcStatement *statement, struct ArrowSchema *schema,
                                          struct AdbcPartitions *partitions, int64_t *rows_affected,
                                          struct AdbcError *error) {
	SetError(error, "Execute Partitions are not supported in Goose");
	return ADBC_STATUS_NOT_IMPLEMENTED;
}

AdbcStatusCode ConnectionCommit(struct AdbcConnection *connection, struct AdbcError *error) {
	if (!connection) {
		SetError(error, "Connection is not set");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	auto conn_wrapper = static_cast<goose::GooseAdbcConnectionWrapper *>(connection->private_data);
	auto conn = reinterpret_cast<goose::Connection *>(conn_wrapper->connection);
	if (!conn->HasActiveTransaction()) {
		SetError(error, "No active transaction, cannot commit");
		return ADBC_STATUS_INVALID_STATE;
	}

	AdbcStatusCode status = ExecuteQuery(conn, "COMMIT", error);
	if (status != ADBC_STATUS_OK) {
		return status;
	}
	return ExecuteQuery(conn, "START TRANSACTION", error);
}

AdbcStatusCode ConnectionRollback(struct AdbcConnection *connection, struct AdbcError *error) {
	if (!connection) {
		SetError(error, "Connection is not set");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	auto conn_wrapper = static_cast<goose::GooseAdbcConnectionWrapper *>(connection->private_data);
	auto conn = reinterpret_cast<goose::Connection *>(conn_wrapper->connection);
	if (!conn->HasActiveTransaction()) {
		SetError(error, "No active transaction, cannot rollback");
		return ADBC_STATUS_INVALID_STATE;
	}

	AdbcStatusCode status = ExecuteQuery(conn, "ROLLBACK", error);
	if (status != ADBC_STATUS_OK) {
		return status;
	}
	return ExecuteQuery(conn, "START TRANSACTION", error);
}

enum class AdbcInfoCode : uint32_t {
	VENDOR_NAME,
	VENDOR_VERSION,
	DRIVER_NAME,
	DRIVER_VERSION,
	DRIVER_ARROW_VERSION,
	DRIVER_ADBC_VERSION,
	UNRECOGNIZED // always the last entry of the enum
};

static AdbcInfoCode ConvertToInfoCode(uint32_t info_code) {
	switch (info_code) {
	case ADBC_INFO_VENDOR_NAME:
		return AdbcInfoCode::VENDOR_NAME;
	case ADBC_INFO_VENDOR_VERSION:
		return AdbcInfoCode::VENDOR_VERSION;
	case ADBC_INFO_DRIVER_NAME:
		return AdbcInfoCode::DRIVER_NAME;
	case ADBC_INFO_DRIVER_VERSION:
		return AdbcInfoCode::DRIVER_VERSION;
	case ADBC_INFO_DRIVER_ARROW_VERSION:
		return AdbcInfoCode::DRIVER_ARROW_VERSION;
	case ADBC_INFO_DRIVER_ADBC_VERSION:
		return AdbcInfoCode::DRIVER_ADBC_VERSION;
	default:
		return AdbcInfoCode::UNRECOGNIZED;
	}
}

AdbcStatusCode ConnectionGetInfo(struct AdbcConnection *connection, const uint32_t *info_codes,
                                 size_t info_codes_length, struct ArrowArrayStream *out, struct AdbcError *error) {
	if (!connection) {
		SetError(error, "Missing connection object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!connection->private_data) {
		SetError(error, "Connection is invalid");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!out) {
		SetError(error, "Output parameter was not provided");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}

	// If 'info_codes' is NULL, we should output all the info codes we recognize
	static constexpr uint32_t DEFAULT_INFO_CODES[] = {ADBC_INFO_VENDOR_NAME,          ADBC_INFO_VENDOR_VERSION,
	                                                  ADBC_INFO_DRIVER_NAME,          ADBC_INFO_DRIVER_VERSION,
	                                                  ADBC_INFO_DRIVER_ARROW_VERSION, ADBC_INFO_DRIVER_ADBC_VERSION};
	const uint32_t *requested_codes = info_codes ? info_codes : DEFAULT_INFO_CODES;
	size_t length = info_codes ? info_codes_length : (sizeof(DEFAULT_INFO_CODES) / sizeof(DEFAULT_INFO_CODES[0]));

	goose::string q = R"EOF(
		select
			name::UINTEGER as info_name,
			info::UNION(
				string_value VARCHAR,
				bool_value BOOL,
				int64_value BIGINT,
				int32_bitmask INTEGER,
				string_list VARCHAR[],
				int32_to_int32_list_map MAP(INTEGER, INTEGER[])
			) as info_value from values
	)EOF";

	goose::string results = "";
	static constexpr const char *INFO_UNION_TYPE = "UNION(string_value VARCHAR, bool_value BOOL, int64_value BIGINT, "
	                                               "int32_bitmask INTEGER, string_list VARCHAR[], "
	                                               "int32_to_int32_list_map MAP(INTEGER, INTEGER[]))";

	for (size_t i = 0; i < length; i++) {
		auto code = goose::NumericCast<uint32_t>(requested_codes[i]);
		auto info_code = ConvertToInfoCode(code);
		switch (info_code) {
		case AdbcInfoCode::VENDOR_NAME: {
			results += goose::StringUtil::Format("(%u, union_value(string_value := 'goose')::%s),",
			                                      (uint32_t)ADBC_INFO_VENDOR_NAME, INFO_UNION_TYPE);
			break;
		}
		case AdbcInfoCode::VENDOR_VERSION: {
			results += goose::StringUtil::Format("(%u, union_value(string_value := '%s')::%s),",
			                                      (uint32_t)ADBC_INFO_VENDOR_VERSION, goose_library_version(),
			                                      INFO_UNION_TYPE);
			break;
		}
		case AdbcInfoCode::DRIVER_NAME: {
			results += goose::StringUtil::Format("(%u, union_value(string_value := 'ADBC Goose Driver')::%s),",
			                                      (uint32_t)ADBC_INFO_DRIVER_NAME, INFO_UNION_TYPE);
			break;
		}
		case AdbcInfoCode::DRIVER_VERSION: {
			results += goose::StringUtil::Format("(%u, union_value(string_value := '%s')::%s),",
			                                      (uint32_t)ADBC_INFO_DRIVER_VERSION, goose_library_version(),
			                                      INFO_UNION_TYPE);
			break;
		}
		case AdbcInfoCode::DRIVER_ARROW_VERSION: {
			// TODO: fill in arrow version
			results += goose::StringUtil::Format("(%u, union_value(string_value := '(unknown)')::%s),",
			                                      (uint32_t)ADBC_INFO_DRIVER_ARROW_VERSION, INFO_UNION_TYPE);
			break;
		}
		case AdbcInfoCode::DRIVER_ADBC_VERSION: {
			results += goose::StringUtil::Format("(%u, union_value(int64_value := %lld::BIGINT)::%s),",
			                                      ADBC_INFO_DRIVER_ADBC_VERSION, (long long)ADBC_VERSION_1_1_0,
			                                      INFO_UNION_TYPE);
			break;
		}
		case AdbcInfoCode::UNRECOGNIZED: {
			// Unrecognized codes are not an error, just ignored
			continue;
		}
		default: {
			// Codes that we have implemented but not handled here are a developer error
			SetError(error, "Info code recognized but not handled");
			return ADBC_STATUS_INTERNAL;
		}
		}
	}
	if (results.empty()) {
		// Add a group of values so the query parses
		q += "(NULL, NULL)";
	} else {
		q += results;
	}
	q += " tbl(name, info)";
	if (results.empty()) {
		// Add an impossible where clause to return an empty result set
		q += " where true = false";
	}
	return QueryInternal(connection, out, q.c_str(), error);
}

AdbcStatusCode ConnectionInit(struct AdbcConnection *connection, struct AdbcDatabase *database,
                              struct AdbcError *error) {
	if (!database) {
		SetError(error, "Missing database object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!database->private_data) {
		SetError(error, "Invalid database");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!connection) {
		SetError(error, "Missing connection object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	auto database_wrapper = static_cast<GooseAdbcDatabaseWrapper *>(database->private_data);
	auto conn_wrapper = static_cast<goose::GooseAdbcConnectionWrapper *>(connection->private_data);
	conn_wrapper->connection = nullptr;

	auto res = goose_connect(database_wrapper->database, &conn_wrapper->connection);
	auto adbc_status = CheckResult(res, error, "Failed to connect to Database");
	if (adbc_status != ADBC_STATUS_OK) {
		return adbc_status;
	}
	// We might have options to set
	auto conn = reinterpret_cast<goose::Connection *>(conn_wrapper->connection);
	return InternalSetOption(*conn, conn_wrapper->options, error);
}

AdbcStatusCode ConnectionRelease(struct AdbcConnection *connection, struct AdbcError *error) {
	if (connection && connection->private_data) {
		auto conn_wrapper = static_cast<goose::GooseAdbcConnectionWrapper *>(connection->private_data);
		auto conn = reinterpret_cast<goose::Connection *>(conn_wrapper->connection);
		goose_disconnect(reinterpret_cast<goose_connection *>(&conn));
		delete conn_wrapper;
		connection->private_data = nullptr;
	}
	return ADBC_STATUS_OK;
}

// some stream callbacks

static int get_schema(struct ArrowArrayStream *stream, struct ArrowSchema *out) {
	if (!stream || !stream->private_data || !out) {
		return GooseError;
	}
	auto result_wrapper = static_cast<GooseAdbcStreamWrapper *>(stream->private_data);
	auto count = goose_column_count(&result_wrapper->result);
	std::vector<goose_logical_type> types(count);

	std::vector<std::string> owned_names;
	owned_names.reserve(count);
	goose::vector<const char *> names(count);
	for (idx_t i = 0; i < count; i++) {
		types[i] = goose_column_logical_type(&result_wrapper->result, i);
		auto column_name = goose_column_name(&result_wrapper->result, i);
		owned_names.emplace_back(column_name);
		names[i] = owned_names.back().c_str();
	}

	auto arrow_options = goose_result_get_arrow_options(&result_wrapper->result);

	auto res = goose_to_arrow_schema(arrow_options, types.data(), names.data(), count, out);
	goose_destroy_arrow_options(&arrow_options);
	for (auto &type : types) {
		goose_destroy_logical_type(&type);
	}
	if (res) {
		goose_destroy_error_data(&res);
		return GooseError;
	}
	return GooseSuccess;
}

static int get_next(struct ArrowArrayStream *stream, struct ArrowArray *out) {
	if (!stream || !stream->private_data || !out) {
		return GooseError;
	}
	out->release = nullptr;
	auto result_wrapper = static_cast<GooseAdbcStreamWrapper *>(stream->private_data);
	auto goose_chunk = goose_fetch_chunk(result_wrapper->result);
	if (!goose_chunk) {
		return GooseSuccess;
	}
	auto arrow_options = goose_result_get_arrow_options(&result_wrapper->result);

	auto conversion_success = goose_data_chunk_to_arrow(arrow_options, goose_chunk, out);
	goose_destroy_arrow_options(&arrow_options);
	goose_destroy_data_chunk(&goose_chunk);

	if (conversion_success) {
		goose_destroy_error_data(&conversion_success);
		return GooseError;
	}
	return GooseSuccess;
}

void release(struct ArrowArrayStream *stream) {
	if (!stream || !stream->release) {
		return;
	}
	auto result_wrapper = reinterpret_cast<GooseAdbcStreamWrapper *>(stream->private_data);
	if (result_wrapper) {
		goose_destroy_result(&result_wrapper->result);
	}
	free(stream->private_data);
	stream->private_data = nullptr;
	stream->release = nullptr;
}

const char *get_last_error(struct ArrowArrayStream *stream) {
	return nullptr;
}

// this is an evil hack, normally we would need a stream factory here, but its probably much easier if the adbc clients
// just hand over a stream

goose::unique_ptr<goose::ArrowArrayStreamWrapper> stream_produce(uintptr_t factory_ptr,
                                                                   goose::ArrowStreamParameters &parameters) {
	// TODO this will ignore any projections or filters but since we don't expose the scan it should be sort of fine
	auto res = goose::make_uniq<goose::ArrowArrayStreamWrapper>();
	res->arrow_array_stream = *reinterpret_cast<ArrowArrayStream *>(factory_ptr);
	return res;
}

void stream_schema(ArrowArrayStream *stream, ArrowSchema &schema) {
	stream->get_schema(stream, &schema);
}

// Helper function to build CREATE TABLE SQL statement
static std::string BuildCreateTableSQL(const char *schema, const char *table_name,
                                       const goose::vector<goose::LogicalType> &types,
                                       const goose::vector<std::string> &names, bool if_not_exists = false) {
	std::ostringstream create_table;
	create_table << "CREATE TABLE ";
	if (if_not_exists) {
		create_table << "IF NOT EXISTS ";
	}
	if (schema) {
		create_table << goose::KeywordHelper::WriteOptionallyQuoted(schema) << ".";
	}
	create_table << goose::KeywordHelper::WriteOptionallyQuoted(table_name) << " (";
	for (idx_t i = 0; i < types.size(); i++) {
		create_table << goose::KeywordHelper::WriteOptionallyQuoted(names[i]);
		create_table << " " << types[i].ToString();
		if (i + 1 < types.size()) {
			create_table << ", ";
		}
	}
	create_table << ");";
	return create_table.str();
}

// Helper function to build DROP TABLE IF EXISTS SQL statement
static std::string BuildDropTableSQL(const char *schema, const char *table_name) {
	std::ostringstream drop_table;
	drop_table << "DROP TABLE IF EXISTS ";
	if (schema) {
		drop_table << goose::KeywordHelper::WriteOptionallyQuoted(schema) << ".";
	}
	drop_table << goose::KeywordHelper::WriteOptionallyQuoted(table_name);
	return drop_table.str();
}

AdbcStatusCode Ingest(goose_connection connection, const char *table_name, const char *schema,
                      struct ArrowArrayStream *input, struct AdbcError *error, IngestionMode ingestion_mode,
                      bool temporary, int64_t *rows_affected) {
	if (!connection) {
		SetError(error, "Missing connection object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!input) {
		SetError(error, "Missing input arrow stream pointer");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!table_name) {
		SetError(error, "Missing database object name");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (schema && temporary) {
		// Temporary option is not supported with ADBC_INGEST_OPTION_TARGET_DB_SCHEMA or
		// ADBC_INGEST_OPTION_TARGET_CATALOG
		SetError(error, "Temporary option is not supported with schema");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}

	goose::ArrowSchemaWrapper arrow_schema_wrapper;
	ConvertedSchemaWrapper out_types;

	input->get_schema(input, &arrow_schema_wrapper.arrow_schema);
	auto res = goose_schema_from_arrow(connection, &arrow_schema_wrapper.arrow_schema, out_types.GetPtr());
	if (res) {
		SetError(error, goose_error_data_message(res));
		goose_destroy_error_data(&res);
		return ADBC_STATUS_INTERNAL;
	}

	auto &d_converted_schema = *reinterpret_cast<goose::ArrowTableSchema *>(out_types.Get());
	auto types = d_converted_schema.GetTypes();
	auto names = d_converted_schema.GetNames();

	// Handle different ingestion modes
	switch (ingestion_mode) {
	case IngestionMode::CREATE: {
		// CREATE mode: Create table, error if already exists
		auto sql = BuildCreateTableSQL(schema, table_name, types, names);
		goose_result result;
		if (goose_query(connection, sql.c_str(), &result) == GooseError) {
			const char *error_msg = goose_result_error(&result);
			// Check if error is about table already existing before destroying result
			bool already_exists = error_msg && std::string(error_msg).find("already exists") != std::string::npos;
			goose_destroy_result(&result);
			if (already_exists) {
				return ADBC_STATUS_ALREADY_EXISTS;
			}
			return ADBC_STATUS_INTERNAL;
		}
		goose_destroy_result(&result);
		break;
	}
	case IngestionMode::APPEND:
		// APPEND mode: No pre-check needed
		// The appender will naturally fail if the table doesn't exist
		break;
	case IngestionMode::REPLACE: {
		// REPLACE mode: Drop table if exists, then create
		auto drop_sql = BuildDropTableSQL(schema, table_name);
		auto create_sql = BuildCreateTableSQL(schema, table_name, types, names);
		goose_result result;
		if (goose_query(connection, drop_sql.c_str(), &result) == GooseError) {
			SetError(error, goose_result_error(&result));
			goose_destroy_result(&result);
			return ADBC_STATUS_INTERNAL;
		}
		goose_destroy_result(&result);
		if (goose_query(connection, create_sql.c_str(), &result) == GooseError) {
			SetError(error, goose_result_error(&result));
			goose_destroy_result(&result);
			return ADBC_STATUS_INTERNAL;
		}
		goose_destroy_result(&result);
		break;
	}
	case IngestionMode::CREATE_APPEND: {
		// CREATE_APPEND mode: Create if not exists, append if exists
		auto sql = BuildCreateTableSQL(schema, table_name, types, names, true);
		goose_result result;
		if (goose_query(connection, sql.c_str(), &result) == GooseError) {
			SetError(error, goose_result_error(&result));
			goose_destroy_result(&result);
			return ADBC_STATUS_INTERNAL;
		}
		goose_destroy_result(&result);
		break;
	}
	}
	AppenderWrapper appender(connection, schema, table_name);
	if (!appender.Valid()) {
		return ADBC_STATUS_INTERNAL;
	}
	goose::ArrowArrayWrapper arrow_array_wrapper;

	// Initialize rows_affected counter if requested
	int64_t affected = 0;

	input->get_next(input, &arrow_array_wrapper.arrow_array);
	while (arrow_array_wrapper.arrow_array.release) {
		DataChunkWrapper out_chunk;
		auto res = goose_data_chunk_from_arrow(connection, &arrow_array_wrapper.arrow_array, out_types.Get(),
		                                        &out_chunk.chunk);
		if (res) {
			SetError(error, goose_error_data_message(res));
			goose_destroy_error_data(&res);
		}
		// Count rows for rows_affected, if a chunk was produced
		if (out_chunk.chunk) {
			auto *chunk = reinterpret_cast<goose::DataChunk *>(out_chunk.chunk);
			affected += static_cast<int64_t>(chunk->size());
		}
		if (goose_append_data_chunk(appender.Get(), out_chunk.chunk) != GooseSuccess) {
			auto error_data = goose_appender_error_data(appender.Get());
			SetError(error, goose_error_data_message(error_data));
			goose_destroy_error_data(&error_data);
			return ADBC_STATUS_INTERNAL;
		}
		arrow_array_wrapper = goose::ArrowArrayWrapper();
		input->get_next(input, &arrow_array_wrapper.arrow_array);
	}
	if (rows_affected) {
		*rows_affected = affected;
	}
	return ADBC_STATUS_OK;
}

AdbcStatusCode StatementNew(struct AdbcConnection *connection, struct AdbcStatement *statement,
                            struct AdbcError *error) {
	if (!connection) {
		SetError(error, "Missing connection object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!connection->private_data) {
		SetError(error, "Invalid connection object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!statement) {
		SetError(error, "Missing statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}

	statement->private_data = nullptr;

	auto statement_wrapper = static_cast<GooseAdbcStatementWrapper *>(malloc(sizeof(GooseAdbcStatementWrapper)));
	if (!statement_wrapper) {
		SetError(error, "Allocation error");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}

	statement->private_data = statement_wrapper;
	auto conn_wrapper = static_cast<goose::GooseAdbcConnectionWrapper *>(connection->private_data);

	statement_wrapper->connection = conn_wrapper->connection;
	statement_wrapper->statement = nullptr;
	statement_wrapper->ingestion_stream.release = nullptr;
	statement_wrapper->ingestion_table_name = nullptr;
	statement_wrapper->db_schema = nullptr;
	statement_wrapper->temporary_table = false;

	statement_wrapper->ingestion_mode = IngestionMode::CREATE;
	return ADBC_STATUS_OK;
}

AdbcStatusCode StatementRelease(struct AdbcStatement *statement, struct AdbcError *error) {
	if (!statement || !statement->private_data) {
		return ADBC_STATUS_OK;
	}
	auto wrapper = static_cast<GooseAdbcStatementWrapper *>(statement->private_data);
	if (wrapper->statement) {
		goose_destroy_prepare(&wrapper->statement);
		wrapper->statement = nullptr;
	}
	if (wrapper->ingestion_stream.release) {
		wrapper->ingestion_stream.release(&wrapper->ingestion_stream);
		wrapper->ingestion_stream.release = nullptr;
	}
	if (wrapper->ingestion_table_name) {
		free(wrapper->ingestion_table_name);
		wrapper->ingestion_table_name = nullptr;
	}
	if (wrapper->db_schema) {
		free(wrapper->db_schema);
		wrapper->db_schema = nullptr;
	}
	free(statement->private_data);
	statement->private_data = nullptr;
	return ADBC_STATUS_OK;
}

AdbcStatusCode StatementGetParameterSchema(struct AdbcStatement *statement, struct ArrowSchema *schema,
                                           struct AdbcError *error) {
	if (!statement) {
		SetError(error, "Missing statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!statement->private_data) {
		SetError(error, "Invalid statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!schema) {
		SetError(error, "Missing schema object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	auto wrapper = static_cast<GooseAdbcStatementWrapper *>(statement->private_data);
	// TODO: we might want to cache this, but then we need to return a deep copy anyways.., so I'm not sure if that
	// would be worth the extra management

	auto prepared_wrapper = reinterpret_cast<goose::PreparedStatementWrapper *>(wrapper->statement);
	if (!prepared_wrapper || !prepared_wrapper->statement || !prepared_wrapper->statement->data) {
		SetError(error, "Invalid prepared statement wrapper");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	auto count = prepared_wrapper->statement->data->properties.parameter_count;
	std::vector<goose_logical_type> types(count);
	std::vector<std::string> owned_names;
	owned_names.reserve(count);
	goose::vector<const char *> names(count);

	for (idx_t i = 0; i < count; i++) {
		// FIXME: we don't support named parameters yet, but when we do, this needs to be updated
		// Every prepared parameter type is UNKNOWN, which we need to map to NULL according to the spec of
		// 'AdbcStatementGetParameterSchema'
		types[i] = goose_create_logical_type(GOOSE_TYPE_SQLNULL);
		auto column_name = std::to_string(i);
		owned_names.emplace_back(column_name);
		names[i] = owned_names.back().c_str();
	}

	goose_arrow_options arrow_options;
	goose_connection_get_arrow_options(wrapper->connection, &arrow_options);

	auto res = goose_to_arrow_schema(arrow_options, types.data(), names.data(), count, schema);

	for (auto &type : types) {
		goose_destroy_logical_type(&type);
	}
	goose_destroy_arrow_options(&arrow_options);

	if (res) {
		SetError(error, goose_error_data_message(res));
		goose_destroy_error_data(&res);
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	return ADBC_STATUS_OK;
}

static AdbcStatusCode IngestToTableFromBoundStream(GooseAdbcStatementWrapper *statement, int64_t *rows_affected,
                                                   AdbcError *error) {
	// See ADBC_INGEST_OPTION_TARGET_TABLE
	D_ASSERT(statement->ingestion_stream.release);
	D_ASSERT(statement->ingestion_table_name);

	// Take the input stream from the statement
	auto stream = statement->ingestion_stream;

	// Ingest into a table from the bound stream
	return Ingest(statement->connection, statement->ingestion_table_name, statement->db_schema, &stream, error,
	              statement->ingestion_mode, statement->temporary_table, rows_affected);
}

AdbcStatusCode StatementExecuteQuery(struct AdbcStatement *statement, struct ArrowArrayStream *out,
                                     int64_t *rows_affected, struct AdbcError *error) {
	if (!statement) {
		SetError(error, "Missing statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!statement->private_data) {
		SetError(error, "Invalid statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	auto wrapper = static_cast<GooseAdbcStatementWrapper *>(statement->private_data);

	// TODO: Set affected rows, careful with early return
	if (rows_affected) {
		*rows_affected = 0;
	}

	const auto has_stream = wrapper->ingestion_stream.release != nullptr;
	const auto to_table = wrapper->ingestion_table_name != nullptr;

	if (has_stream && to_table) {
		return IngestToTableFromBoundStream(wrapper, rows_affected, error);
	}
	auto stream_wrapper = static_cast<GooseAdbcStreamWrapper *>(malloc(sizeof(GooseAdbcStreamWrapper)));
	if (!stream_wrapper) {
		SetError(error, "Allocation error");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	std::memset(&stream_wrapper->result, 0, sizeof(stream_wrapper->result));
	// Only process the stream if there are parameters to bind
	auto prepared_statement_params = reinterpret_cast<goose::PreparedStatementWrapper *>(wrapper->statement)
	                                     ->statement->data->properties.parameter_count;
	if (has_stream && prepared_statement_params > 0) {
		// A stream was bound to the statement, use that to bind parameters
		ArrowArrayStream stream = wrapper->ingestion_stream;
		ConvertedSchemaWrapper out_types;
		goose::ArrowSchemaWrapper arrow_schema_wrapper;
		stream.get_schema(&stream, &arrow_schema_wrapper.arrow_schema);
		try {
			auto res =
			    goose_schema_from_arrow(wrapper->connection, &arrow_schema_wrapper.arrow_schema, out_types.GetPtr());
			if (res) {
				SetError(error, goose_error_data_message(res));
				goose_destroy_error_data(&res);
			}
		} catch (...) {
			free(stream_wrapper);
			return ADBC_STATUS_INTERNAL;
		}

		goose::ArrowArrayWrapper arrow_array_wrapper;

		stream.get_next(&stream, &arrow_array_wrapper.arrow_array);

		while (arrow_array_wrapper.arrow_array.release) {
			// This is a valid arrow array, let's make it into a data chunk
			DataChunkWrapper out_chunk;
			auto res_conv = goose_data_chunk_from_arrow(wrapper->connection, &arrow_array_wrapper.arrow_array,
			                                             out_types.Get(), &out_chunk.chunk);
			if (res_conv) {
				SetError(error, goose_error_data_message(res_conv));
				goose_destroy_error_data(&res_conv);
				return ADBC_STATUS_INVALID_ARGUMENT;
			}
			if (!out_chunk.chunk) {
				SetError(error, "Please provide a non-empty chunk to be bound");
				free(stream_wrapper);
				return ADBC_STATUS_INVALID_ARGUMENT;
			}
			auto chunk = reinterpret_cast<goose::DataChunk *>(out_chunk.chunk);
			if (chunk->size() == 0) {
				SetError(error, "Please provide a non-empty chunk to be bound");
				free(stream_wrapper);
				return ADBC_STATUS_INVALID_ARGUMENT;
			}
			if (chunk->size() != 1) {
				// TODO: add support for binding multiple rows
				SetError(error, "Binding multiple rows at once is not supported yet");
				free(stream_wrapper);
				return ADBC_STATUS_NOT_IMPLEMENTED;
			}
			if (chunk->ColumnCount() > prepared_statement_params) {
				SetError(error, "Input data has more column than prepared statement has parameters");
				free(stream_wrapper);
				return ADBC_STATUS_INVALID_ARGUMENT;
			}
			goose_clear_bindings(wrapper->statement);
			for (idx_t col_idx = 0; col_idx < chunk->ColumnCount(); col_idx++) {
				auto val = chunk->GetValue(col_idx, 0);
				auto gval = reinterpret_cast<goose_value>(&val);
				auto res = goose_bind_value(wrapper->statement, 1 + col_idx, gval);
				if (res != GooseSuccess) {
					SetError(error, goose_prepare_error(wrapper->statement));
					free(stream_wrapper);
					return ADBC_STATUS_INVALID_ARGUMENT;
				}
			}
			// Destroy any previous result before overwriting to avoid leaks
			goose_destroy_result(&stream_wrapper->result);
			auto res = goose_execute_prepared(wrapper->statement, &stream_wrapper->result);
			if (res != GooseSuccess) {
				SetError(error, goose_result_error(&stream_wrapper->result));
				goose_destroy_result(&stream_wrapper->result);
				free(stream_wrapper);
				return ADBC_STATUS_INVALID_ARGUMENT;
			}
			// Recreate wrappers for next iteration
			arrow_array_wrapper = goose::ArrowArrayWrapper();
			stream.get_next(&stream, &arrow_array_wrapper.arrow_array);
		}
	} else {
		auto res = goose_execute_prepared(wrapper->statement, &stream_wrapper->result);
		if (res != GooseSuccess) {
			SetError(error, goose_result_error(&stream_wrapper->result));
			goose_destroy_result(&stream_wrapper->result);
			free(stream_wrapper);
			return ADBC_STATUS_INVALID_ARGUMENT;
		}
	}

	// Set rows_affected for queries (if not already set by ingestion path)
	if (rows_affected && !(has_stream && to_table)) {
		// For DML queries (INSERT/UPDATE/DELETE), goose_rows_changed() returns the count
		// For SELECT queries, goose_rows_changed() returns 0
		auto rows_changed = goose_rows_changed(&stream_wrapper->result);
		if (rows_changed > 0) {
			// This was a DML query
			*rows_affected = static_cast<int64_t>(rows_changed);
		} else {
			// This is a SELECT or other query that returns a result set
			// Return -1 to indicate unknown, as results are streamed
			*rows_affected = -1;
		}
	}

	if (out) {
		// We pass ownership of the statement private data to our stream
		out->private_data = stream_wrapper;
		out->get_schema = get_schema;
		out->get_next = get_next;
		out->release = release;
		out->get_last_error = get_last_error;
	} else {
		// Caller didn't request a stream; clean up resources
		goose_destroy_result(&stream_wrapper->result);
		free(stream_wrapper);
	}

	return ADBC_STATUS_OK;
}

// this is a nop for us
AdbcStatusCode StatementPrepare(struct AdbcStatement *statement, struct AdbcError *error) {
	if (!statement) {
		SetError(error, "Missing statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!statement->private_data) {
		SetError(error, "Invalid statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	return ADBC_STATUS_OK;
}

AdbcStatusCode StatementSetSqlQuery(struct AdbcStatement *statement, const char *query, struct AdbcError *error) {
	if (!statement) {
		SetError(error, "Missing statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!statement->private_data) {
		SetError(error, "Invalid statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!query) {
		SetError(error, "Missing query");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}

	auto wrapper = static_cast<GooseAdbcStatementWrapper *>(statement->private_data);
	if (wrapper->ingestion_stream.release) {
		// Release any resources currently held by the ingestion stream before we overwrite it
		wrapper->ingestion_stream.release(&wrapper->ingestion_stream);
		wrapper->ingestion_stream.release = nullptr;
	}
	if (wrapper->statement) {
		goose_destroy_prepare(&wrapper->statement);
		wrapper->statement = nullptr;
	}
	goose_extracted_statements extracted_statements = nullptr;
	auto extract_statements_size = goose_extract_statements(wrapper->connection, query, &extracted_statements);
	auto error_msg_extract_statements = goose_extract_statements_error(extracted_statements);
	if (error_msg_extract_statements != nullptr) {
		// Things went wrong when executing internal prepared statement
		SetError(error, error_msg_extract_statements);
		goose_destroy_extracted(&extracted_statements);
		return ADBC_STATUS_INTERNAL;
	}
	// Now lets loop over the statements, and execute every one
	for (idx_t i = 0; i < extract_statements_size - 1; i++) {
		goose_prepared_statement statement_internal = nullptr;
		auto res =
		    goose_prepare_extracted_statement(wrapper->connection, extracted_statements, i, &statement_internal);
		auto error_msg = goose_prepare_error(statement_internal);
		auto adbc_status = CheckResult(res, error, error_msg);
		if (adbc_status != ADBC_STATUS_OK) {
			goose_destroy_prepare(&statement_internal);
			goose_destroy_extracted(&extracted_statements);
			return adbc_status;
		}
		// Execute
		goose_arrow out_result;
		res = goose_execute_prepared_arrow(statement_internal, &out_result);
		if (res != GooseSuccess) {
			SetError(error, goose_query_arrow_error(out_result));
			goose_destroy_arrow(&out_result);
			goose_destroy_prepare(&statement_internal);
			goose_destroy_extracted(&extracted_statements);
			return ADBC_STATUS_INVALID_ARGUMENT;
		}
		goose_destroy_arrow(&out_result);
		goose_destroy_prepare(&statement_internal);
	}

	// Final statement (returned to caller)
	auto res = goose_prepare_extracted_statement(wrapper->connection, extracted_statements,
	                                              extract_statements_size - 1, &wrapper->statement);
	auto error_msg = goose_prepare_error(wrapper->statement);
	goose_destroy_extracted(&extracted_statements);
	return CheckResult(res, error, error_msg);
}

AdbcStatusCode StatementBind(struct AdbcStatement *statement, struct ArrowArray *values, struct ArrowSchema *schemas,
                             struct AdbcError *error) {
	if (!statement) {
		SetError(error, "Missing statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!statement->private_data) {
		SetError(error, "Invalid statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!values) {
		SetError(error, "Missing values object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!schemas) {
		SetError(error, "Invalid schemas object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}

	auto wrapper = static_cast<GooseAdbcStatementWrapper *>(statement->private_data);
	if (wrapper->ingestion_stream.release) {
		// Free the stream that was previously bound
		wrapper->ingestion_stream.release(&wrapper->ingestion_stream);
	}
	auto status = BatchToArrayStream(values, schemas, &wrapper->ingestion_stream, error);
	return status;
}

AdbcStatusCode StatementBindStream(struct AdbcStatement *statement, struct ArrowArrayStream *values,
                                   struct AdbcError *error) {
	if (!statement) {
		SetError(error, "Missing statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!statement->private_data) {
		SetError(error, "Invalid statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!values) {
		SetError(error, "Missing values object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}

	auto wrapper = static_cast<GooseAdbcStatementWrapper *>(statement->private_data);
	if (wrapper->ingestion_stream.release) {
		// Release any resources currently held by the ingestion stream before we overwrite it
		wrapper->ingestion_stream.release(&wrapper->ingestion_stream);
	}
	wrapper->ingestion_stream = *values;
	values->release = nullptr;
	return ADBC_STATUS_OK;
}

AdbcStatusCode StatementSetOption(struct AdbcStatement *statement, const char *key, const char *value,
                                  struct AdbcError *error) {
	if (!statement) {
		SetError(error, "Missing statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!statement->private_data) {
		SetError(error, "Invalid statement object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}
	if (!key) {
		SetError(error, "Missing key object");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}

	auto wrapper = static_cast<GooseAdbcStatementWrapper *>(statement->private_data);

	if (strcmp(key, ADBC_INGEST_OPTION_TARGET_TABLE) == 0) {
		wrapper->ingestion_table_name = strdup(value);
		wrapper->temporary_table = false;
		return ADBC_STATUS_OK;
	}
	if (strcmp(key, ADBC_INGEST_OPTION_TEMPORARY) == 0) {
		if (strcmp(value, ADBC_OPTION_VALUE_ENABLED) == 0) {
			if (wrapper->db_schema) {
				SetError(error, "Temporary option is not supported with schema");
				return ADBC_STATUS_INVALID_ARGUMENT;
			}
			wrapper->temporary_table = true;
			return ADBC_STATUS_OK;
		} else if (strcmp(value, ADBC_OPTION_VALUE_DISABLED) == 0) {
			wrapper->temporary_table = false;
			return ADBC_STATUS_OK;
		} else {
			SetError(
			    error,
			    "ADBC_INGEST_OPTION_TEMPORARY, can only be ADBC_OPTION_VALUE_ENABLED or ADBC_OPTION_VALUE_DISABLED");
			return ADBC_STATUS_INVALID_ARGUMENT;
		}
	}

	if (strcmp(key, ADBC_INGEST_OPTION_TARGET_DB_SCHEMA) == 0) {
		if (wrapper->temporary_table) {
			SetError(error, "Temporary option is not supported with schema");
			return ADBC_STATUS_INVALID_ARGUMENT;
		}
		wrapper->db_schema = strdup(value);
		return ADBC_STATUS_OK;
	}

	if (strcmp(key, ADBC_INGEST_OPTION_MODE) == 0) {
		if (strcmp(value, ADBC_INGEST_OPTION_MODE_CREATE) == 0) {
			wrapper->ingestion_mode = IngestionMode::CREATE;
			return ADBC_STATUS_OK;
		} else if (strcmp(value, ADBC_INGEST_OPTION_MODE_APPEND) == 0) {
			wrapper->ingestion_mode = IngestionMode::APPEND;
			return ADBC_STATUS_OK;
		} else if (strcmp(value, ADBC_INGEST_OPTION_MODE_REPLACE) == 0) {
			wrapper->ingestion_mode = IngestionMode::REPLACE;
			return ADBC_STATUS_OK;
		} else if (strcmp(value, ADBC_INGEST_OPTION_MODE_CREATE_APPEND) == 0) {
			wrapper->ingestion_mode = IngestionMode::CREATE_APPEND;
			return ADBC_STATUS_OK;
		} else {
			SetError(error, "Invalid ingestion mode");
			return ADBC_STATUS_INVALID_ARGUMENT;
		}
	}
	goose::stringstream ss;
	ss << "Statement Set Option " << key << " is not yet accepted by Goose";
	SetError(error, ss.str());
	return ADBC_STATUS_INVALID_ARGUMENT;
}

std::string createFilter(const char *input) {
	if (input) {
		auto quoted = goose::KeywordHelper::WriteQuoted(input, '\'');
		return quoted;
	}
	return "'%'";
}

AdbcStatusCode ConnectionGetObjects(struct AdbcConnection *connection, int depth, const char *catalog,
                                    const char *db_schema, const char *table_name, const char **table_type,
                                    const char *column_name, struct ArrowArrayStream *out, struct AdbcError *error) {
	std::string catalog_filter = createFilter(catalog);
	std::string db_schema_filter = createFilter(db_schema);
	std::string table_name_filter = createFilter(table_name);
	std::string column_name_filter = createFilter(column_name);
	std::string table_type_condition = "";
	if (table_type && table_type[0]) {
		table_type_condition = " AND table_type IN (";
		for (int i = 0; table_type[i]; ++i) {
			if ((strcmp(table_type[i], "LOCAL TABLE") != 0) && (strcmp(table_type[i], "BASE TABLE") != 0) &&
			    (strcmp(table_type[i], "VIEW") != 0)) {
				goose::stringstream ss;
				ss << "Table type must be \"LOCAL TABLE\", \"BASE TABLE\" or "
				   << "\"VIEW\": \"" << table_type[i] << "\"";
				SetError(error, ss.str());
				return ADBC_STATUS_INVALID_ARGUMENT;
			}
			if (i > 0) {
				table_type_condition += ", ";
			}
			table_type_condition += createFilter(table_type[i]);
		}
		table_type_condition += ")";
	}

	std::string query;
	switch (depth) {
	case ADBC_OBJECT_DEPTH_CATALOGS:
		// Return metadata on catalogs.
		query = goose::StringUtil::Format(R"(
				SELECT
					catalog_name,
					[]::STRUCT(
						db_schema_name VARCHAR,
						db_schema_tables STRUCT(
							table_name VARCHAR,
							table_type VARCHAR,
							table_columns STRUCT(
								column_name VARCHAR,
								ordinal_position INTEGER,
								remarks VARCHAR,
								xdbc_data_type SMALLINT,
								xdbc_type_name VARCHAR,
								xdbc_column_size INTEGER,
								xdbc_decimal_digits SMALLINT,
								xdbc_num_prec_radix SMALLINT,
								xdbc_nullable SMALLINT,
								xdbc_column_def VARCHAR,
								xdbc_sql_data_type SMALLINT,
								xdbc_datetime_sub SMALLINT,
								xdbc_char_octet_length INTEGER,
								xdbc_is_nullable VARCHAR,
								xdbc_scope_catalog VARCHAR,
								xdbc_scope_schema VARCHAR,
								xdbc_scope_table VARCHAR,
								xdbc_is_autoincrement BOOLEAN,
								xdbc_is_generatedcolumn BOOLEAN
							)[],
							table_constraints STRUCT(
								constraint_name VARCHAR,
								constraint_type VARCHAR,
								constraint_column_names VARCHAR[],
								constraint_column_usage STRUCT(fk_catalog VARCHAR, fk_db_schema VARCHAR, fk_table VARCHAR, fk_column_name VARCHAR)[]
							)[]
						)[]
					)[] catalog_db_schemas
				FROM
					information_schema.schemata
				WHERE catalog_name LIKE %s
				GROUP BY catalog_name
				)",
		                                   catalog_filter);
		break;
	case ADBC_OBJECT_DEPTH_DB_SCHEMAS:
		// Return metadata on catalogs and schemas.
		query = goose::StringUtil::Format(R"(
				WITH db_schemas AS (
					SELECT
						catalog_name,
						schema_name,
					FROM information_schema.schemata
					WHERE schema_name LIKE %s
				)

				SELECT
					catalog_name,
					LIST({
						db_schema_name: schema_name,
						db_schema_tables: []::STRUCT(
							table_name VARCHAR,
							table_type VARCHAR,
							table_columns STRUCT(
								column_name VARCHAR,
								ordinal_position INTEGER,
								remarks VARCHAR,
								xdbc_data_type SMALLINT,
								xdbc_type_name VARCHAR,
								xdbc_column_size INTEGER,
								xdbc_decimal_digits SMALLINT,
								xdbc_num_prec_radix SMALLINT,
								xdbc_nullable SMALLINT,
								xdbc_column_def VARCHAR,
								xdbc_sql_data_type SMALLINT,
								xdbc_datetime_sub SMALLINT,
								xdbc_char_octet_length INTEGER,
								xdbc_is_nullable VARCHAR,
								xdbc_scope_catalog VARCHAR,
								xdbc_scope_schema VARCHAR,
								xdbc_scope_table VARCHAR,
								xdbc_is_autoincrement BOOLEAN,
								xdbc_is_generatedcolumn BOOLEAN
							)[],
							table_constraints STRUCT(
								constraint_name VARCHAR,
								constraint_type VARCHAR,
								constraint_column_names VARCHAR[],
								constraint_column_usage STRUCT(fk_catalog VARCHAR, fk_db_schema VARCHAR, fk_table VARCHAR, fk_column_name VARCHAR)[]
							)[]
						)[],
					}) FILTER (dbs.schema_name is not null) catalog_db_schemas
				FROM
					information_schema.schemata
				LEFT JOIN db_schemas dbs
				USING (catalog_name, schema_name)
				WHERE catalog_name LIKE %s
				GROUP BY catalog_name
				)",
		                                   db_schema_filter, catalog_filter);
		break;
	case ADBC_OBJECT_DEPTH_TABLES:
		// Return metadata on catalogs, schemas, and tables.
		query = goose::StringUtil::Format(R"(
				WITH tables AS (
					SELECT
						table_catalog catalog_name,
						table_schema schema_name,
						LIST({
							table_name: table_name,
							table_type: table_type,
							table_columns: []::STRUCT(
								column_name VARCHAR,
								ordinal_position INTEGER,
								remarks VARCHAR,
								xdbc_data_type SMALLINT,
								xdbc_type_name VARCHAR,
								xdbc_column_size INTEGER,
								xdbc_decimal_digits SMALLINT,
								xdbc_num_prec_radix SMALLINT,
								xdbc_nullable SMALLINT,
								xdbc_column_def VARCHAR,
								xdbc_sql_data_type SMALLINT,
								xdbc_datetime_sub SMALLINT,
								xdbc_char_octet_length INTEGER,
								xdbc_is_nullable VARCHAR,
								xdbc_scope_catalog VARCHAR,
								xdbc_scope_schema VARCHAR,
								xdbc_scope_table VARCHAR,
								xdbc_is_autoincrement BOOLEAN,
								xdbc_is_generatedcolumn BOOLEAN
							)[],
							table_constraints: []::STRUCT(
								constraint_name VARCHAR,
								constraint_type VARCHAR,
								constraint_column_names VARCHAR[],
								constraint_column_usage STRUCT(fk_catalog VARCHAR, fk_db_schema VARCHAR, fk_table VARCHAR, fk_column_name VARCHAR)[]
							)[],
						}) db_schema_tables
					FROM information_schema.tables
					WHERE table_name LIKE %s%s
					GROUP BY table_catalog, table_schema
				),
				db_schemas AS (
					SELECT
						catalog_name,
						schema_name,
						db_schema_tables,
					FROM information_schema.schemata
					LEFT JOIN tables
					USING (catalog_name, schema_name)
					WHERE schema_name LIKE %s
				)

				SELECT
					catalog_name,
					LIST({
						db_schema_name: schema_name,
						db_schema_tables: db_schema_tables,
					}) FILTER (dbs.schema_name is not null) catalog_db_schemas
				FROM
					information_schema.schemata
				LEFT JOIN db_schemas dbs
				USING (catalog_name, schema_name)
				WHERE catalog_name LIKE %s
				GROUP BY catalog_name
				)",
		                                   table_name_filter, table_type_condition, db_schema_filter, catalog_filter);
		break;
	case ADBC_OBJECT_DEPTH_COLUMNS:
		// Return metadata on catalogs, schemas, tables, and columns.
		query = goose::StringUtil::Format(R"(
				WITH columns AS (
					SELECT
						table_catalog,
						table_schema,
						table_name,
						LIST({
							column_name: column_name,
							ordinal_position: ordinal_position,
							remarks: '',
							xdbc_data_type: NULL::SMALLINT,
							xdbc_type_name: NULL::VARCHAR,
							xdbc_column_size: NULL::INTEGER,
							xdbc_decimal_digits: NULL::SMALLINT,
							xdbc_num_prec_radix: NULL::SMALLINT,
							xdbc_nullable: NULL::SMALLINT,
							xdbc_column_def: NULL::VARCHAR,
							xdbc_sql_data_type: NULL::SMALLINT,
							xdbc_datetime_sub: NULL::SMALLINT,
							xdbc_char_octet_length: NULL::INTEGER,
							xdbc_is_nullable: NULL::VARCHAR,
							xdbc_scope_catalog: NULL::VARCHAR,
							xdbc_scope_schema: NULL::VARCHAR,
							xdbc_scope_table: NULL::VARCHAR,
							xdbc_is_autoincrement: NULL::BOOLEAN,
							xdbc_is_generatedcolumn: NULL::BOOLEAN,
						}) table_columns
					FROM information_schema.columns
					WHERE column_name LIKE %s
					GROUP BY table_catalog, table_schema, table_name
				),
				constraints AS (
					SELECT
						database_name AS table_catalog,
						schema_name AS table_schema,
						table_name,
						LIST({
							constraint_name: constraint_name,
							constraint_type: constraint_type,
							constraint_column_names: constraint_column_names,
							constraint_column_usage: list_transform(
								referenced_column_names,
								lambda name: {
									fk_catalog: database_name,
									fk_db_schema: schema_name,
									fk_table: referenced_table,
									fk_column_name: name,
								}
							)
						}) table_constraints
					FROM goose_constraints()
					WHERE
						constraint_type NOT IN ('NOT NULL') AND
						list_has_any(
							constraint_column_names,
							list_filter(
								constraint_column_names,
								lambda name: name LIKE %s
							)
						)
					GROUP BY database_name, schema_name, table_name
				),
				tables AS (
					SELECT
						table_catalog catalog_name,
						table_schema schema_name,
						LIST({
							table_name: table_name,
							table_type: table_type,
							table_columns: table_columns,
							table_constraints: table_constraints,
						}) db_schema_tables
					FROM information_schema.tables
					LEFT JOIN columns
					USING (table_catalog, table_schema, table_name)
					LEFT JOIN constraints
					USING (table_catalog, table_schema, table_name)
					WHERE table_name LIKE %s%s
					GROUP BY table_catalog, table_schema
				),
				db_schemas AS (
					SELECT
						catalog_name,
						schema_name,
						db_schema_tables,
					FROM information_schema.schemata
					LEFT JOIN tables
					USING (catalog_name, schema_name)
					WHERE schema_name LIKE %s
				)

				SELECT
					catalog_name,
					LIST({
						db_schema_name: schema_name,
						db_schema_tables: db_schema_tables,
					}) FILTER (dbs.schema_name is not null) catalog_db_schemas
				FROM
					information_schema.schemata
				LEFT JOIN db_schemas dbs
				USING (catalog_name, schema_name)
				WHERE catalog_name LIKE %s
				GROUP BY catalog_name
				)",
		                                   column_name_filter, column_name_filter, table_name_filter,
		                                   table_type_condition, db_schema_filter, catalog_filter);
		break;
	default:
		SetError(error, "Invalid value of Depth");
		return ADBC_STATUS_INVALID_ARGUMENT;
	}

	return QueryInternal(connection, out, query.c_str(), error);
}

AdbcStatusCode ConnectionGetTableTypes(struct AdbcConnection *connection, struct ArrowArrayStream *out,
                                       struct AdbcError *error) {
	const auto q = "SELECT DISTINCT table_type FROM information_schema.tables ORDER BY table_type";
	return QueryInternal(connection, out, q, error);
}

} // namespace goose_adbc

static void ReleaseError(struct AdbcError *error) {
	if (error) {
		if (error->message)
			delete[] error->message;
		error->message = nullptr;
		error->release = nullptr;
	}
}

void SetError(struct AdbcError *error, const std::string &message) {
	if (!error)
		return;
	if (error->message) {
		// Append
		std::string buffer = error->message;
		buffer.reserve(buffer.size() + message.size() + 1);
		buffer += '\n';
		buffer += message;
		error->release(error);

		error->message = new char[buffer.size() + 1];
		buffer.copy(error->message, buffer.size());
		error->message[buffer.size()] = '\0';
	} else {
		error->message = new char[message.size() + 1];
		message.copy(error->message, message.size());
		error->message[message.size()] = '\0';
	}
	error->release = ReleaseError;
}
