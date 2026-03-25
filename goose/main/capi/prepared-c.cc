#include <goose/main/capi/capi_internal.h>
#include <goose/main/query_result.h>
#include <goose/main/prepared_statement_data.h>
#include <goose/common/types/decimal.h>
#include <goose/common/uhugeint.h>
#include <goose/common/optional_ptr.h>
#include <goose/common/case_insensitive_map.h>

using goose::case_insensitive_map_t;
using goose::Connection;
using goose::date_t;
using goose::dtime_t;
using goose::ErrorData;
using goose::ExtractStatementsWrapper;
using goose::hugeint_t;
using goose::LogicalType;
using goose::MaterializedQueryResult;
using goose::optional_ptr;
using goose::PreparedStatementWrapper;
using goose::QueryResultType;
using goose::StringUtil;
using goose::timestamp_t;
using goose::uhugeint_t;
using goose::Value;

idx_t goose_extract_statements(goose_connection connection, const char *query,
                               goose_extracted_statements *out_extracted_statements) {
    if (!connection || !query || !out_extracted_statements) {
        return 0;
    }
    auto wrapper = new ExtractStatementsWrapper();
    Connection *conn = reinterpret_cast<Connection *>(connection);
    try {
        wrapper->statements = conn->ExtractStatements(query);
    } catch (const std::exception &ex) {
        ErrorData error(ex);
        wrapper->error = error.Message();
    }

    *out_extracted_statements = (goose_extracted_statements) wrapper;
    return wrapper->statements.size();
}

goose_state goose_prepare_extracted_statement(goose_connection connection,
                                              goose_extracted_statements extracted_statements, idx_t index,
                                              goose_prepared_statement *out_prepared_statement) {
    Connection *conn = reinterpret_cast<Connection *>(connection);
    auto source_wrapper = (ExtractStatementsWrapper *) extracted_statements;

    if (!connection || !out_prepared_statement || index >= source_wrapper->statements.size()) {
        return GooseError;
    }
    auto wrapper = new PreparedStatementWrapper();
    try {
        wrapper->statement = conn->Prepare(std::move(source_wrapper->statements[index]));
        *out_prepared_statement = (goose_prepared_statement) wrapper;
        return wrapper->statement->HasError() ? GooseError : GooseSuccess;
    } catch (...) {
        delete wrapper;
        return GooseError;
    }
}

const char *goose_extract_statements_error(goose_extracted_statements extracted_statements) {
    auto wrapper = (ExtractStatementsWrapper *) extracted_statements;
    if (!wrapper || wrapper->error.empty()) {
        return nullptr;
    }
    return wrapper->error.c_str();
}

goose_state goose_prepare(goose_connection connection, const char *query,
                          goose_prepared_statement *out_prepared_statement) {
    if (!connection || !query || !out_prepared_statement) {
        return GooseError;
    }
    auto wrapper = new PreparedStatementWrapper();
    Connection *conn = reinterpret_cast<Connection *>(connection);
    try {
        wrapper->statement = conn->Prepare(query);
        *out_prepared_statement = reinterpret_cast<goose_prepared_statement>(wrapper);
        return !wrapper->statement->HasError() ? GooseSuccess : GooseError;
    } catch (...) {
        delete wrapper;
        return GooseError;
    }
}

const char *goose_prepare_error(goose_prepared_statement prepared_statement) {
    auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared_statement);
    if (!wrapper) {
        return nullptr;
    }
    if (!wrapper->success) {
        return wrapper->error_data.Message().c_str();
    }
    if (!wrapper->statement || !wrapper->statement->HasError()) {
        return nullptr;
    }
    return wrapper->statement->error.Message().c_str();
}

idx_t goose_nparams(goose_prepared_statement prepared_statement) {
    auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared_statement);
    if (!wrapper || !wrapper->statement || wrapper->statement->HasError()) {
        return 0;
    }
    return wrapper->statement->named_param_map.size();
}

static goose::string goose_parameter_name_internal(goose_prepared_statement prepared_statement, idx_t index) {
    auto wrapper = (PreparedStatementWrapper *) prepared_statement;
    if (!wrapper || !wrapper->statement || wrapper->statement->HasError()) {
        return goose::string();
    }
    if (index > wrapper->statement->named_param_map.size()) {
        return goose::string();
    }
    for (auto &item: wrapper->statement->named_param_map) {
        auto &identifier = item.first;
        auto &param_idx = item.second;
        if (param_idx == index) {
            // Found the matching parameter
            return identifier;
        }
    }
    // No parameter was found with this index
    return goose::string();
}

const char *goose_parameter_name(goose_prepared_statement prepared_statement, idx_t index) {
    auto identifier = goose_parameter_name_internal(prepared_statement, index);
    if (identifier == goose::string()) {
        return NULL;
    }
    return strdup(identifier.c_str());
}

goose_type goose_param_type(goose_prepared_statement prepared_statement, idx_t param_idx) {
    auto logical_type = goose_param_logical_type(prepared_statement, param_idx);
    if (!logical_type) {
        return GOOSE_TYPE_INVALID;
    }

    auto type = goose_get_type_id(logical_type);
    goose_destroy_logical_type(&logical_type);

    return type;
}

goose_logical_type goose_param_logical_type(goose_prepared_statement prepared_statement, idx_t param_idx) {
    auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared_statement);
    if (!wrapper || !wrapper->statement || wrapper->statement->HasError()) {
        return nullptr;
    }

    auto identifier = goose_parameter_name_internal(prepared_statement, param_idx);
    if (identifier == goose::string()) {
        return nullptr;
    }

    LogicalType param_type;

    if (wrapper->statement->data->TryGetType(identifier, param_type)) {
        return reinterpret_cast<goose_logical_type>(new LogicalType(param_type));
    }
    // The value_map is gone after executing the prepared statement
    // See if this is the case and we still have a value registered for it
    auto it = wrapper->values.find(identifier);
    if (it != wrapper->values.end()) {
        return reinterpret_cast<goose_logical_type>(new LogicalType(it->second.return_type));
    }
    return nullptr;
}

goose_state goose_clear_bindings(goose_prepared_statement prepared_statement) {
    auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared_statement);
    if (!wrapper || !wrapper->statement || wrapper->statement->HasError()) {
        return GooseError;
    }
    wrapper->values.clear();
    return GooseSuccess;
}

idx_t goose_prepared_statement_column_count(goose_prepared_statement prepared_statement) {
    auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared_statement);
    if (!wrapper || !wrapper->statement || wrapper->statement->HasError()) {
        return 0;
    }
    return wrapper->statement->ColumnCount();
}

const char *goose_prepared_statement_column_name(goose_prepared_statement prepared_statement, idx_t col_idx) {
    auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared_statement);
    if (!wrapper || !wrapper->statement || wrapper->statement->HasError()) {
        return nullptr;
    }
    auto &names = wrapper->statement->GetNames();

    if (col_idx >= names.size()) {
        return nullptr;
    }
    return strdup(names[col_idx].c_str());
}

goose_logical_type goose_prepared_statement_column_logical_type(goose_prepared_statement prepared_statement,
                                                                idx_t col_idx) {
    auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared_statement);
    if (!wrapper || !wrapper->statement || wrapper->statement->HasError()) {
        return nullptr;
    }
    auto types = wrapper->statement->GetTypes();
    if (col_idx >= types.size()) {
        return nullptr;
    }
    return reinterpret_cast<goose_logical_type>(new LogicalType(types[col_idx]));
}

goose_type goose_prepared_statement_column_type(goose_prepared_statement prepared_statement, idx_t col_idx) {
    auto logical_type = goose_prepared_statement_column_logical_type(prepared_statement, col_idx);
    if (!logical_type) {
        return GOOSE_TYPE_INVALID;
    }

    auto type = goose_get_type_id(logical_type);
    goose_destroy_logical_type(&logical_type);

    return type;
}

goose_state goose_bind_value(goose_prepared_statement prepared_statement, idx_t param_idx, goose_value val) {
    auto value = reinterpret_cast<Value *>(val);
    auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared_statement);
    if (!wrapper || !wrapper->statement || wrapper->statement->HasError()) {
        return GooseError;
    }
    if (param_idx <= 0 || param_idx > wrapper->statement->named_param_map.size()) {
        wrapper->error_data =
                goose::InvalidInputException("Can not bind to parameter number %d, statement only has %d parameter(s)",
                                             param_idx, wrapper->statement->named_param_map.size());
        wrapper->success = false;
        return GooseError;
    }
    auto identifier = goose_parameter_name_internal(prepared_statement, param_idx);
    wrapper->values[identifier] = goose::BoundParameterData(*value);
    return GooseSuccess;
}

goose_state goose_bind_parameter_index(goose_prepared_statement prepared_statement, idx_t *param_idx_out,
                                       const char *name_p) {
    auto wrapper = (PreparedStatementWrapper *) prepared_statement;
    if (!wrapper || !wrapper->statement || wrapper->statement->HasError()) {
        return GooseError;
    }
    if (!name_p || !param_idx_out) {
        return GooseError;
    }
    auto name = std::string(name_p);
    for (auto &pair: wrapper->statement->named_param_map) {
        if (goose::StringUtil::CIEquals(pair.first, name)) {
            *param_idx_out = pair.second;
            return GooseSuccess;
        }
    }
    return GooseError;
}

goose_state goose_bind_boolean(goose_prepared_statement prepared_statement, idx_t param_idx, bool val) {
    auto value = Value::BOOLEAN(val);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_int8(goose_prepared_statement prepared_statement, idx_t param_idx, int8_t val) {
    auto value = Value::TINYINT(val);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_int16(goose_prepared_statement prepared_statement, idx_t param_idx, int16_t val) {
    auto value = Value::SMALLINT(val);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_int32(goose_prepared_statement prepared_statement, idx_t param_idx, int32_t val) {
    auto value = Value::INTEGER(val);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_int64(goose_prepared_statement prepared_statement, idx_t param_idx, int64_t val) {
    auto value = Value::BIGINT(val);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

static hugeint_t goose_internal_hugeint(goose_hugeint val) {
    hugeint_t internal;
    internal.lower = val.lower;
    internal.upper = val.upper;
    return internal;
}

static uhugeint_t goose_internal_uhugeint(goose_uhugeint val) {
    uhugeint_t internal;
    internal.lower = val.lower;
    internal.upper = val.upper;
    return internal;
}

goose_state goose_bind_hugeint(goose_prepared_statement prepared_statement, idx_t param_idx, goose_hugeint val) {
    auto value = Value::HUGEINT(goose_internal_hugeint(val));
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_uhugeint(goose_prepared_statement prepared_statement, idx_t param_idx, goose_uhugeint val) {
    auto value = Value::UHUGEINT(goose_internal_uhugeint(val));
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_uint8(goose_prepared_statement prepared_statement, idx_t param_idx, uint8_t val) {
    auto value = Value::UTINYINT(val);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_uint16(goose_prepared_statement prepared_statement, idx_t param_idx, uint16_t val) {
    auto value = Value::USMALLINT(val);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_uint32(goose_prepared_statement prepared_statement, idx_t param_idx, uint32_t val) {
    auto value = Value::UINTEGER(val);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_uint64(goose_prepared_statement prepared_statement, idx_t param_idx, uint64_t val) {
    auto value = Value::UBIGINT(val);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_float(goose_prepared_statement prepared_statement, idx_t param_idx, float val) {
    auto value = Value::FLOAT(val);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_double(goose_prepared_statement prepared_statement, idx_t param_idx, double val) {
    auto value = Value::DOUBLE(val);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_date(goose_prepared_statement prepared_statement, idx_t param_idx, goose_date val) {
    auto value = Value::DATE(date_t(val.days));
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_time(goose_prepared_statement prepared_statement, idx_t param_idx, goose_time val) {
    auto value = Value::TIME(dtime_t(val.micros));
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_timestamp(goose_prepared_statement prepared_statement, idx_t param_idx,
                                 goose_timestamp val) {
    auto value = Value::TIMESTAMP(timestamp_t(val.micros));
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_timestamp_tz(goose_prepared_statement prepared_statement, idx_t param_idx,
                                    goose_timestamp val) {
    auto value = Value::TIMESTAMPTZ(goose::timestamp_tz_t(val.micros));
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_interval(goose_prepared_statement prepared_statement, idx_t param_idx, goose_interval val) {
    auto value = Value::INTERVAL(val.months, val.days, val.micros);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_varchar(goose_prepared_statement prepared_statement, idx_t param_idx, const char *val) {
    try {
        auto value = Value(val);
        return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
    } catch (...) {
        return GooseError;
    }
}

goose_state goose_bind_varchar_length(goose_prepared_statement prepared_statement, idx_t param_idx, const char *val,
                                      idx_t length) {
    try {
        auto value = Value(std::string(val, length));
        return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
    } catch (...) {
        return GooseError;
    }
}

goose_state goose_bind_decimal(goose_prepared_statement prepared_statement, idx_t param_idx, goose_decimal val) {
    auto hugeint_val = goose_internal_hugeint(val.value);
    if (val.width > goose::Decimal::MAX_WIDTH_INT64) {
        auto value = Value::DECIMAL(hugeint_val, val.width, val.scale);
        return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
    }
    auto value = hugeint_val.lower;
    auto gval = Value::DECIMAL((int64_t) value, val.width, val.scale);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &gval);
}

goose_state goose_bind_blob(goose_prepared_statement prepared_statement, idx_t param_idx, const void *data,
                            idx_t length) {
    auto value = Value::BLOB(goose::const_data_ptr_cast(data), length);
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_bind_null(goose_prepared_statement prepared_statement, idx_t param_idx) {
    auto value = Value();
    return goose_bind_value(prepared_statement, param_idx, (goose_value) &value);
}

goose_state goose_execute_prepared(goose_prepared_statement prepared_statement, goose_result *out_result) {
    auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared_statement);
    if (!wrapper || !wrapper->statement || wrapper->statement->HasError()) {
        return GooseError;
    }

    goose::unique_ptr<goose::QueryResult> result;
    try {
        result = wrapper->statement->Execute(wrapper->values, false);
    } catch (...) {
        return GooseError;
    }
    return GooseTranslateResult(std::move(result), out_result);
}

goose_state goose_execute_prepared_streaming(goose_prepared_statement prepared_statement,
                                             goose_result *out_result) {
    auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared_statement);
    if (!wrapper || !wrapper->statement || wrapper->statement->HasError()) {
        return GooseError;
    }

    try {
        auto result = wrapper->statement->Execute(wrapper->values, true);
        return GooseTranslateResult(std::move(result), out_result);
    } catch (...) {
        return GooseError;
    }
}

goose_statement_type goose_prepared_statement_type(goose_prepared_statement statement) {
    if (!statement) {
        return GOOSE_STATEMENT_TYPE_INVALID;
    }
    auto stmt = reinterpret_cast<PreparedStatementWrapper *>(statement);

    return StatementTypeToC(stmt->statement->GetStatementType());
}

template<class T>
void goose_destroy(void **wrapper) {
    if (!wrapper) {
        return;
    }

    auto casted = (T *) *wrapper;
    if (casted) {
        delete casted;
    }
    *wrapper = nullptr;
}

void goose_destroy_extracted(goose_extracted_statements *extracted_statements) {
    goose_destroy<ExtractStatementsWrapper>(reinterpret_cast<void **>(extracted_statements));
}

void goose_destroy_prepare(goose_prepared_statement *prepared_statement) {
    goose_destroy<PreparedStatementWrapper>(reinterpret_cast<void **>(prepared_statement));
}
