#include <goose/main/capi/capi_internal.h>

namespace goose {

LogicalTypeId LogicalTypeIdFromC(const goose_type type) {
	switch (type) {
	case GOOSE_TYPE_INVALID:
		return LogicalTypeId::INVALID;
	case GOOSE_TYPE_BOOLEAN:
		return LogicalTypeId::BOOLEAN;
	case GOOSE_TYPE_TINYINT:
		return LogicalTypeId::TINYINT;
	case GOOSE_TYPE_SMALLINT:
		return LogicalTypeId::SMALLINT;
	case GOOSE_TYPE_INTEGER:
		return LogicalTypeId::INTEGER;
	case GOOSE_TYPE_BIGINT:
		return LogicalTypeId::BIGINT;
	case GOOSE_TYPE_UTINYINT:
		return LogicalTypeId::UTINYINT;
	case GOOSE_TYPE_USMALLINT:
		return LogicalTypeId::USMALLINT;
	case GOOSE_TYPE_UINTEGER:
		return LogicalTypeId::UINTEGER;
	case GOOSE_TYPE_UBIGINT:
		return LogicalTypeId::UBIGINT;
	case GOOSE_TYPE_FLOAT:
		return LogicalTypeId::FLOAT;
	case GOOSE_TYPE_DOUBLE:
		return LogicalTypeId::DOUBLE;
	case GOOSE_TYPE_TIMESTAMP:
		return LogicalTypeId::TIMESTAMP;
	case GOOSE_TYPE_DATE:
		return LogicalTypeId::DATE;
	case GOOSE_TYPE_TIME:
		return LogicalTypeId::TIME;
	case GOOSE_TYPE_INTERVAL:
		return LogicalTypeId::INTERVAL;
	case GOOSE_TYPE_HUGEINT:
		return LogicalTypeId::HUGEINT;
	case GOOSE_TYPE_UHUGEINT:
		return LogicalTypeId::UHUGEINT;
	case GOOSE_TYPE_VARCHAR:
		return LogicalTypeId::VARCHAR;
	case GOOSE_TYPE_BLOB:
		return LogicalTypeId::BLOB;
	case GOOSE_TYPE_DECIMAL:
		return LogicalTypeId::DECIMAL;
	case GOOSE_TYPE_TIMESTAMP_S:
		return LogicalTypeId::TIMESTAMP_SEC;
	case GOOSE_TYPE_TIMESTAMP_MS:
		return LogicalTypeId::TIMESTAMP_MS;
	case GOOSE_TYPE_TIMESTAMP_NS:
		return LogicalTypeId::TIMESTAMP_NS;
	case GOOSE_TYPE_ENUM:
		return LogicalTypeId::ENUM;
	case GOOSE_TYPE_LIST:
		return LogicalTypeId::LIST;
	case GOOSE_TYPE_STRUCT:
		return LogicalTypeId::STRUCT;
	case GOOSE_TYPE_MAP:
		return LogicalTypeId::MAP;
	case GOOSE_TYPE_ARRAY:
		return LogicalTypeId::ARRAY;
	case GOOSE_TYPE_UUID:
		return LogicalTypeId::UUID;
	case GOOSE_TYPE_UNION:
		return LogicalTypeId::UNION;
	case GOOSE_TYPE_BIT:
		return LogicalTypeId::BIT;
	case GOOSE_TYPE_TIME_TZ:
		return LogicalTypeId::TIME_TZ;
	case GOOSE_TYPE_TIMESTAMP_TZ:
		return LogicalTypeId::TIMESTAMP_TZ;
	case GOOSE_TYPE_ANY:
		return LogicalTypeId::ANY;
	case GOOSE_TYPE_BIGNUM:
		return LogicalTypeId::BIGNUM;
	case GOOSE_TYPE_SQLNULL:
		return LogicalTypeId::SQLNULL;
	case GOOSE_TYPE_STRING_LITERAL:
		return LogicalTypeId::STRING_LITERAL;
	case GOOSE_TYPE_INTEGER_LITERAL:
		return LogicalTypeId::INTEGER_LITERAL;
	default: // LCOV_EXCL_START
		D_ASSERT(0);
		return LogicalTypeId::INVALID;
	} // LCOV_EXCL_STOP
}

goose_type LogicalTypeIdToC(const LogicalTypeId type) {
	switch (type) {
	case LogicalTypeId::INVALID:
		return GOOSE_TYPE_INVALID;
	case LogicalTypeId::UNKNOWN:
		return GOOSE_TYPE_INVALID;
	case LogicalTypeId::BOOLEAN:
		return GOOSE_TYPE_BOOLEAN;
	case LogicalTypeId::TINYINT:
		return GOOSE_TYPE_TINYINT;
	case LogicalTypeId::SMALLINT:
		return GOOSE_TYPE_SMALLINT;
	case LogicalTypeId::INTEGER:
		return GOOSE_TYPE_INTEGER;
	case LogicalTypeId::BIGINT:
		return GOOSE_TYPE_BIGINT;
	case LogicalTypeId::UTINYINT:
		return GOOSE_TYPE_UTINYINT;
	case LogicalTypeId::USMALLINT:
		return GOOSE_TYPE_USMALLINT;
	case LogicalTypeId::UINTEGER:
		return GOOSE_TYPE_UINTEGER;
	case LogicalTypeId::UBIGINT:
		return GOOSE_TYPE_UBIGINT;
	case LogicalTypeId::HUGEINT:
		return GOOSE_TYPE_HUGEINT;
	case LogicalTypeId::UHUGEINT:
		return GOOSE_TYPE_UHUGEINT;
	case LogicalTypeId::FLOAT:
		return GOOSE_TYPE_FLOAT;
	case LogicalTypeId::DOUBLE:
		return GOOSE_TYPE_DOUBLE;
	case LogicalTypeId::TIMESTAMP:
		return GOOSE_TYPE_TIMESTAMP;
	case LogicalTypeId::TIMESTAMP_TZ:
		return GOOSE_TYPE_TIMESTAMP_TZ;
	case LogicalTypeId::TIMESTAMP_SEC:
		return GOOSE_TYPE_TIMESTAMP_S;
	case LogicalTypeId::TIMESTAMP_MS:
		return GOOSE_TYPE_TIMESTAMP_MS;
	case LogicalTypeId::TIMESTAMP_NS:
		return GOOSE_TYPE_TIMESTAMP_NS;
	case LogicalTypeId::DATE:
		return GOOSE_TYPE_DATE;
	case LogicalTypeId::TIME:
		return GOOSE_TYPE_TIME;
	case LogicalTypeId::TIME_TZ:
		return GOOSE_TYPE_TIME_TZ;
	case LogicalTypeId::VARCHAR:
		return GOOSE_TYPE_VARCHAR;
	case LogicalTypeId::BLOB:
		return GOOSE_TYPE_BLOB;
	case LogicalTypeId::BIT:
		return GOOSE_TYPE_BIT;
	case LogicalTypeId::BIGNUM:
		return GOOSE_TYPE_BIGNUM;
	case LogicalTypeId::INTERVAL:
		return GOOSE_TYPE_INTERVAL;
	case LogicalTypeId::DECIMAL:
		return GOOSE_TYPE_DECIMAL;
	case LogicalTypeId::ENUM:
		return GOOSE_TYPE_ENUM;
	case LogicalTypeId::LIST:
		return GOOSE_TYPE_LIST;
	case LogicalTypeId::STRUCT:
		return GOOSE_TYPE_STRUCT;
	case LogicalTypeId::MAP:
		return GOOSE_TYPE_MAP;
	case LogicalTypeId::UNION:
		return GOOSE_TYPE_UNION;
	case LogicalTypeId::UUID:
		return GOOSE_TYPE_UUID;
	case LogicalTypeId::ARRAY:
		return GOOSE_TYPE_ARRAY;
	case LogicalTypeId::ANY:
		return GOOSE_TYPE_ANY;
	case LogicalTypeId::SQLNULL:
		return GOOSE_TYPE_SQLNULL;
	case LogicalTypeId::STRING_LITERAL:
		return GOOSE_TYPE_STRING_LITERAL;
	case LogicalTypeId::INTEGER_LITERAL:
		return GOOSE_TYPE_INTEGER_LITERAL;
	default: // LCOV_EXCL_START
		D_ASSERT(0);
		return GOOSE_TYPE_INVALID;
	} // LCOV_EXCL_STOP
}

idx_t GetCTypeSize(const goose_type type) {
	switch (type) {
	case GOOSE_TYPE_BOOLEAN:
		return sizeof(bool);
	case GOOSE_TYPE_TINYINT:
		return sizeof(int8_t);
	case GOOSE_TYPE_SMALLINT:
		return sizeof(int16_t);
	case GOOSE_TYPE_INTEGER:
		return sizeof(int32_t);
	case GOOSE_TYPE_BIGINT:
		return sizeof(int64_t);
	case GOOSE_TYPE_UTINYINT:
		return sizeof(uint8_t);
	case GOOSE_TYPE_USMALLINT:
		return sizeof(uint16_t);
	case GOOSE_TYPE_UINTEGER:
		return sizeof(uint32_t);
	case GOOSE_TYPE_UBIGINT:
		return sizeof(uint64_t);
	case GOOSE_TYPE_UHUGEINT:
	case GOOSE_TYPE_HUGEINT:
	case GOOSE_TYPE_UUID:
		return sizeof(goose_hugeint);
	case GOOSE_TYPE_FLOAT:
		return sizeof(float);
	case GOOSE_TYPE_DOUBLE:
		return sizeof(double);
	case GOOSE_TYPE_DATE:
		return sizeof(goose_date);
	case GOOSE_TYPE_TIME:
		return sizeof(goose_time);
	case GOOSE_TYPE_TIMESTAMP:
	case GOOSE_TYPE_TIMESTAMP_TZ:
	case GOOSE_TYPE_TIMESTAMP_S:
	case GOOSE_TYPE_TIMESTAMP_MS:
	case GOOSE_TYPE_TIMESTAMP_NS:
		return sizeof(goose_timestamp);
	case GOOSE_TYPE_VARCHAR:
		return sizeof(const char *);
	case GOOSE_TYPE_BLOB:
		return sizeof(goose_blob);
	case GOOSE_TYPE_INTERVAL:
		return sizeof(goose_interval);
	case GOOSE_TYPE_DECIMAL:
		return sizeof(goose_hugeint);
	default: // LCOV_EXCL_START
		// Unsupported nested or complex type. Internally, we set the null mask to NULL.
		// This is a deprecated code path. Use the Vector Interface for nested and complex types.
		return 0;
	} // LCOV_EXCL_STOP
}

goose_statement_type StatementTypeToC(const StatementType type) {
	switch (type) {
	case StatementType::SELECT_STATEMENT:
		return GOOSE_STATEMENT_TYPE_SELECT;
	case StatementType::INVALID_STATEMENT:
		return GOOSE_STATEMENT_TYPE_INVALID;
	case StatementType::INSERT_STATEMENT:
		return GOOSE_STATEMENT_TYPE_INSERT;
	case StatementType::UPDATE_STATEMENT:
		return GOOSE_STATEMENT_TYPE_UPDATE;
	case StatementType::EXPLAIN_STATEMENT:
		return GOOSE_STATEMENT_TYPE_EXPLAIN;
	case StatementType::DELETE_STATEMENT:
		return GOOSE_STATEMENT_TYPE_DELETE;
	case StatementType::PREPARE_STATEMENT:
		return GOOSE_STATEMENT_TYPE_PREPARE;
	case StatementType::CREATE_STATEMENT:
		return GOOSE_STATEMENT_TYPE_CREATE;
	case StatementType::EXECUTE_STATEMENT:
		return GOOSE_STATEMENT_TYPE_EXECUTE;
	case StatementType::ALTER_STATEMENT:
		return GOOSE_STATEMENT_TYPE_ALTER;
	case StatementType::TRANSACTION_STATEMENT:
		return GOOSE_STATEMENT_TYPE_TRANSACTION;
	case StatementType::COPY_STATEMENT:
		return GOOSE_STATEMENT_TYPE_COPY;
	case StatementType::ANALYZE_STATEMENT:
		return GOOSE_STATEMENT_TYPE_ANALYZE;
	case StatementType::VARIABLE_SET_STATEMENT:
		return GOOSE_STATEMENT_TYPE_VARIABLE_SET;
	case StatementType::CREATE_FUNC_STATEMENT:
		return GOOSE_STATEMENT_TYPE_CREATE_FUNC;
	case StatementType::DROP_STATEMENT:
		return GOOSE_STATEMENT_TYPE_DROP;
	case StatementType::EXPORT_STATEMENT:
		return GOOSE_STATEMENT_TYPE_EXPORT;
	case StatementType::PRAGMA_STATEMENT:
		return GOOSE_STATEMENT_TYPE_PRAGMA;
	case StatementType::VACUUM_STATEMENT:
		return GOOSE_STATEMENT_TYPE_VACUUM;
	case StatementType::CALL_STATEMENT:
		return GOOSE_STATEMENT_TYPE_CALL;
	case StatementType::SET_STATEMENT:
		return GOOSE_STATEMENT_TYPE_SET;
	case StatementType::LOAD_STATEMENT:
		return GOOSE_STATEMENT_TYPE_LOAD;
	case StatementType::RELATION_STATEMENT:
		return GOOSE_STATEMENT_TYPE_RELATION;
	case StatementType::EXTENSION_STATEMENT:
		return GOOSE_STATEMENT_TYPE_EXTENSION;
	case StatementType::LOGICAL_PLAN_STATEMENT:
		return GOOSE_STATEMENT_TYPE_LOGICAL_PLAN;
	case StatementType::ATTACH_STATEMENT:
		return GOOSE_STATEMENT_TYPE_ATTACH;
	case StatementType::DETACH_STATEMENT:
		return GOOSE_STATEMENT_TYPE_DETACH;
	case StatementType::MULTI_STATEMENT:
		return GOOSE_STATEMENT_TYPE_MULTI;
	default:
		return GOOSE_STATEMENT_TYPE_INVALID;
	}
}

goose_error_type ErrorTypeToC(const ExceptionType type) {
	switch (type) {
	case ExceptionType::INVALID:
		return GOOSE_ERROR_INVALID;
	case ExceptionType::OUT_OF_RANGE:
		return GOOSE_ERROR_OUT_OF_RANGE;
	case ExceptionType::CONVERSION:
		return GOOSE_ERROR_CONVERSION;
	case ExceptionType::UNKNOWN_TYPE:
		return GOOSE_ERROR_UNKNOWN_TYPE;
	case ExceptionType::DECIMAL:
		return GOOSE_ERROR_DECIMAL;
	case ExceptionType::MISMATCH_TYPE:
		return GOOSE_ERROR_MISMATCH_TYPE;
	case ExceptionType::DIVIDE_BY_ZERO:
		return GOOSE_ERROR_DIVIDE_BY_ZERO;
	case ExceptionType::OBJECT_SIZE:
		return GOOSE_ERROR_OBJECT_SIZE;
	case ExceptionType::INVALID_TYPE:
		return GOOSE_ERROR_INVALID_TYPE;
	case ExceptionType::SERIALIZATION:
		return GOOSE_ERROR_SERIALIZATION;
	case ExceptionType::TRANSACTION:
		return GOOSE_ERROR_TRANSACTION;
	case ExceptionType::NOT_IMPLEMENTED:
		return GOOSE_ERROR_NOT_IMPLEMENTED;
	case ExceptionType::EXPRESSION:
		return GOOSE_ERROR_EXPRESSION;
	case ExceptionType::CATALOG:
		return GOOSE_ERROR_CATALOG;
	case ExceptionType::PARSER:
		return GOOSE_ERROR_PARSER;
	case ExceptionType::PLANNER:
		return GOOSE_ERROR_PLANNER;
	case ExceptionType::SCHEDULER:
		return GOOSE_ERROR_SCHEDULER;
	case ExceptionType::EXECUTOR:
		return GOOSE_ERROR_EXECUTOR;
	case ExceptionType::CONSTRAINT:
		return GOOSE_ERROR_CONSTRAINT;
	case ExceptionType::INDEX:
		return GOOSE_ERROR_INDEX;
	case ExceptionType::STAT:
		return GOOSE_ERROR_STAT;
	case ExceptionType::CONNECTION:
		return GOOSE_ERROR_CONNECTION;
	case ExceptionType::SYNTAX:
		return GOOSE_ERROR_SYNTAX;
	case ExceptionType::SETTINGS:
		return GOOSE_ERROR_SETTINGS;
	case ExceptionType::BINDER:
		return GOOSE_ERROR_BINDER;
	case ExceptionType::NETWORK:
		return GOOSE_ERROR_NETWORK;
	case ExceptionType::OPTIMIZER:
		return GOOSE_ERROR_OPTIMIZER;
	case ExceptionType::NULL_POINTER:
		return GOOSE_ERROR_NULL_POINTER;
	case ExceptionType::IO:
		return GOOSE_ERROR_IO;
	case ExceptionType::INTERRUPT:
		return GOOSE_ERROR_INTERRUPT;
	case ExceptionType::FATAL:
		return GOOSE_ERROR_FATAL;
	case ExceptionType::INTERNAL:
		return GOOSE_ERROR_INTERNAL;
	case ExceptionType::INVALID_INPUT:
		return GOOSE_ERROR_INVALID_INPUT;
	case ExceptionType::OUT_OF_MEMORY:
		return GOOSE_ERROR_OUT_OF_MEMORY;
	case ExceptionType::PERMISSION:
		return GOOSE_ERROR_PERMISSION;
	case ExceptionType::PARAMETER_NOT_RESOLVED:
		return GOOSE_ERROR_PARAMETER_NOT_RESOLVED;
	case ExceptionType::PARAMETER_NOT_ALLOWED:
		return GOOSE_ERROR_PARAMETER_NOT_ALLOWED;
	case ExceptionType::DEPENDENCY:
		return GOOSE_ERROR_DEPENDENCY;
	case ExceptionType::HTTP:
		return GOOSE_ERROR_HTTP;
	case ExceptionType::MISSING_EXTENSION:
		return GOOSE_ERROR_MISSING_EXTENSION;
	case ExceptionType::AUTOLOAD:
		return GOOSE_ERROR_AUTOLOAD;
	case ExceptionType::SEQUENCE:
		return GOOSE_ERROR_SEQUENCE;
	case ExceptionType::INVALID_CONFIGURATION:
		return GOOSE_INVALID_CONFIGURATION;
	default:
		return GOOSE_ERROR_INVALID;
	}
}

ExceptionType ErrorTypeFromC(const goose_error_type type) {
	switch (type) {
	case GOOSE_ERROR_INVALID:
		return ExceptionType::INVALID;
	case GOOSE_ERROR_OUT_OF_RANGE:
		return ExceptionType::OUT_OF_RANGE;
	case GOOSE_ERROR_CONVERSION:
		return ExceptionType::CONVERSION;
	case GOOSE_ERROR_UNKNOWN_TYPE:
		return ExceptionType::UNKNOWN_TYPE;
	case GOOSE_ERROR_DECIMAL:
		return ExceptionType::DECIMAL;
	case GOOSE_ERROR_MISMATCH_TYPE:
		return ExceptionType::MISMATCH_TYPE;
	case GOOSE_ERROR_DIVIDE_BY_ZERO:
		return ExceptionType::DIVIDE_BY_ZERO;
	case GOOSE_ERROR_OBJECT_SIZE:
		return ExceptionType::OBJECT_SIZE;
	case GOOSE_ERROR_INVALID_TYPE:
		return ExceptionType::INVALID_TYPE;
	case GOOSE_ERROR_SERIALIZATION:
		return ExceptionType::SERIALIZATION;
	case GOOSE_ERROR_TRANSACTION:
		return ExceptionType::TRANSACTION;
	case GOOSE_ERROR_NOT_IMPLEMENTED:
		return ExceptionType::NOT_IMPLEMENTED;
	case GOOSE_ERROR_EXPRESSION:
		return ExceptionType::EXPRESSION;
	case GOOSE_ERROR_CATALOG:
		return ExceptionType::CATALOG;
	case GOOSE_ERROR_PARSER:
		return ExceptionType::PARSER;
	case GOOSE_ERROR_PLANNER:
		return ExceptionType::PLANNER;
	case GOOSE_ERROR_SCHEDULER:
		return ExceptionType::SCHEDULER;
	case GOOSE_ERROR_EXECUTOR:
		return ExceptionType::EXECUTOR;
	case GOOSE_ERROR_CONSTRAINT:
		return ExceptionType::CONSTRAINT;
	case GOOSE_ERROR_INDEX:
		return ExceptionType::INDEX;
	case GOOSE_ERROR_STAT:
		return ExceptionType::STAT;
	case GOOSE_ERROR_CONNECTION:
		return ExceptionType::CONNECTION;
	case GOOSE_ERROR_SYNTAX:
		return ExceptionType::SYNTAX;
	case GOOSE_ERROR_SETTINGS:
		return ExceptionType::SETTINGS;
	case GOOSE_ERROR_BINDER:
		return ExceptionType::BINDER;
	case GOOSE_ERROR_NETWORK:
		return ExceptionType::NETWORK;
	case GOOSE_ERROR_OPTIMIZER:
		return ExceptionType::OPTIMIZER;
	case GOOSE_ERROR_NULL_POINTER:
		return ExceptionType::NULL_POINTER;
	case GOOSE_ERROR_IO:
		return ExceptionType::IO;
	case GOOSE_ERROR_INTERRUPT:
		return ExceptionType::INTERRUPT;
	case GOOSE_ERROR_FATAL:
		return ExceptionType::FATAL;
	case GOOSE_ERROR_INTERNAL:
		return ExceptionType::INTERNAL;
	case GOOSE_ERROR_INVALID_INPUT:
		return ExceptionType::INVALID_INPUT;
	case GOOSE_ERROR_OUT_OF_MEMORY:
		return ExceptionType::OUT_OF_MEMORY;
	case GOOSE_ERROR_PERMISSION:
		return ExceptionType::PERMISSION;
	case GOOSE_ERROR_PARAMETER_NOT_RESOLVED:
		return ExceptionType::PARAMETER_NOT_RESOLVED;
	case GOOSE_ERROR_PARAMETER_NOT_ALLOWED:
		return ExceptionType::PARAMETER_NOT_ALLOWED;
	case GOOSE_ERROR_DEPENDENCY:
		return ExceptionType::DEPENDENCY;
	case GOOSE_ERROR_HTTP:
		return ExceptionType::HTTP;
	case GOOSE_ERROR_MISSING_EXTENSION:
		return ExceptionType::MISSING_EXTENSION;
	case GOOSE_ERROR_AUTOLOAD:
		return ExceptionType::AUTOLOAD;
	case GOOSE_ERROR_SEQUENCE:
		return ExceptionType::SEQUENCE;
	case GOOSE_INVALID_CONFIGURATION:
		return ExceptionType::INVALID_CONFIGURATION;
	default:
		return ExceptionType::INVALID;
	}
}

} // namespace goose

void *goose_malloc(size_t size) {
	return malloc(size);
}

void goose_free(void *ptr) {
	free(ptr);
}

idx_t goose_vector_size() {
	return STANDARD_VECTOR_SIZE;
}

bool goose_string_is_inlined(goose_string_t string_p) {
	static_assert(sizeof(goose_string_t) == sizeof(goose::string_t),
	              "goose_string_t should have the same memory layout as goose::string_t");
	auto &string = *reinterpret_cast<goose::string_t *>(&string_p);
	return string.IsInlined();
}

uint32_t goose_string_t_length(goose_string_t string_p) {
	static_assert(sizeof(goose_string_t) == sizeof(goose::string_t),
	              "goose_string_t should have the same memory layout as goose::string_t");
	auto &string = *reinterpret_cast<goose::string_t *>(&string_p);
	return static_cast<uint32_t>(string.GetSize());
}

const char *goose_string_t_data(goose_string_t *string_p) {
	static_assert(sizeof(goose_string_t) == sizeof(goose::string_t),
	              "goose_string_t should have the same memory layout as goose::string_t");
	auto &string = *reinterpret_cast<goose::string_t *>(string_p);
	return string.GetData();
}
