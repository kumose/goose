#include <goose/main/capi/capi_internal.h>
#include <goose/common/uhugeint.h>

using goose::Appender;
using goose::AppenderWrapper;
using goose::BaseAppender;
using goose::Connection;
using goose::date_t;
using goose::dtime_t;
using goose::ErrorData;
using goose::ErrorDataWrapper;
using goose::hugeint_t;
using goose::interval_t;
using goose::string_t;
using goose::timestamp_t;
using goose::uhugeint_t;

goose_state goose_appender_create(goose_connection connection, const char *schema, const char *table,
                                    goose_appender *out_appender) {
	return goose_appender_create_ext(connection, INVALID_CATALOG, schema, table, out_appender);
}

goose_state goose_appender_create_ext(goose_connection connection, const char *catalog, const char *schema,
                                        const char *table, goose_appender *out_appender) {
	Connection *conn = reinterpret_cast<Connection *>(connection);

	if (!connection || !table || !out_appender) {
		return GooseError;
	}
	if (catalog == nullptr) {
		catalog = INVALID_CATALOG;
	}
	if (schema == nullptr) {
		schema = DEFAULT_SCHEMA;
	}

	auto wrapper = new AppenderWrapper();
	*out_appender = reinterpret_cast<goose_appender>(wrapper);
	try {
		wrapper->appender = goose::make_uniq<Appender>(*conn, catalog, schema, table);
	} catch (std::exception &ex) {
		wrapper->error_data = ErrorData(ex);
		return GooseError;
	} catch (...) { // LCOV_EXCL_START
		wrapper->error_data = ErrorData("Unknown create appender error");
		return GooseError;
	} // LCOV_EXCL_STOP
	return GooseSuccess;
}

goose_state goose_appender_create_query(goose_connection connection, const char *query, idx_t column_count,
                                          goose_logical_type *types_p, const char *table_name_p,
                                          const char **column_names_p, goose_appender *out_appender) {
	Connection *conn = reinterpret_cast<Connection *>(connection);

	if (!connection || !query || !column_count || !types_p) {
		return GooseError;
	}
	goose::vector<goose::LogicalType> types;
	goose::vector<goose::string> column_names;
	goose::string table_name;
	for (idx_t c = 0; c < column_count; ++c) {
		if (!types_p[c]) {
			return GooseError;
		}
		types.push_back(*reinterpret_cast<goose::LogicalType *>(types_p[c]));
	}
	if (table_name_p) {
		table_name = table_name_p;
	}
	if (column_names_p) {
		for (idx_t c = 0; c < column_count; ++c) {
			if (!column_names_p[c]) {
				return GooseError;
			}
			column_names.push_back(column_names_p[c]);
		}
	}

	auto wrapper = new AppenderWrapper();
	*out_appender = reinterpret_cast<goose_appender>(wrapper);
	try {
		wrapper->appender = goose::make_uniq<goose::QueryAppender>(*conn, query, std::move(types),
		                                                             std::move(column_names), std::move(table_name));
	} catch (std::exception &ex) {
		wrapper->error_data = ErrorData(ex);
		return GooseError;
	} catch (...) { // LCOV_EXCL_START
		wrapper->error_data = ErrorData("Unknown create appender error");
		return GooseError;
	} // LCOV_EXCL_STOP
	return GooseSuccess;
}

goose_state goose_appender_destroy(goose_appender *appender) {
	if (!appender || !*appender) {
		return GooseError;
	}
	auto state = goose_appender_close(*appender);
	auto wrapper = reinterpret_cast<AppenderWrapper *>(*appender);
	if (wrapper) {
		delete wrapper;
	}
	*appender = nullptr;
	return state;
}

template <class FUN>
goose_state goose_appender_run_function(goose_appender appender, FUN &&function) {
	if (!appender) {
		return GooseError;
	}
	auto wrapper = reinterpret_cast<AppenderWrapper *>(appender);
	if (!wrapper->appender) {
		wrapper->error_data = ErrorData("not a valid appender");
		return GooseError;
	}
	try {
		function(*wrapper->appender);
	} catch (std::exception &ex) {
		wrapper->error_data = ErrorData(ex);
		return GooseError;
	} catch (...) { // LCOV_EXCL_START
		wrapper->error_data = ErrorData("Unknown appender error");
		return GooseError;
	} // LCOV_EXCL_STOP
	return GooseSuccess;
}

goose_state goose_appender_add_column(goose_appender appender_p, const char *name) {
	return goose_appender_run_function(appender_p, [&](BaseAppender &appender) { appender.AddColumn(name); });
}

goose_state goose_appender_clear_columns(goose_appender appender_p) {
	return goose_appender_run_function(appender_p, [&](BaseAppender &appender) { appender.ClearColumns(); });
}

const char *goose_appender_error(goose_appender appender) {
	if (!appender) {
		return nullptr;
	}
	auto wrapper = reinterpret_cast<AppenderWrapper *>(appender);
	if (!wrapper->error_data.HasError()) {
		return nullptr;
	}
	return wrapper->error_data.RawMessage().c_str();
}

goose_error_data goose_appender_error_data(goose_appender appender) {
	auto errorDataWrapper = new ErrorDataWrapper();
	if (!appender) {
		return reinterpret_cast<goose_error_data>(errorDataWrapper);
	}

	auto appenderWrapper = reinterpret_cast<AppenderWrapper *>(appender);
	errorDataWrapper->error_data = appenderWrapper->error_data;
	return reinterpret_cast<goose_error_data>(errorDataWrapper);
}

goose_state goose_appender_begin_row(goose_appender appender) {
	return GooseSuccess;
}

goose_state goose_appender_end_row(goose_appender appender_p) {
	return goose_appender_run_function(appender_p, [&](BaseAppender &appender) { appender.EndRow(); });
}

template <class T>
goose_state goose_append_internal(goose_appender appender, T value) {
	if (!appender) {
		return GooseError;
	}
	auto *appender_instance = reinterpret_cast<AppenderWrapper *>(appender);
	try {
		appender_instance->appender->Append<T>(value);
	} catch (std::exception &ex) {
		appender_instance->error_data = ErrorData(ex);
		return GooseError;
	} catch (...) {
		return GooseError;
	}
	return GooseSuccess;
}

goose_state goose_append_default(goose_appender appender) {
	if (!appender) {
		return GooseError;
	}
	auto *appender_instance = reinterpret_cast<AppenderWrapper *>(appender);

	try {
		appender_instance->appender->AppendDefault();
	} catch (std::exception &ex) {
		appender_instance->error_data = ErrorData(ex);
		return GooseError;
	} catch (...) {
		return GooseError;
	}
	return GooseSuccess;
}

goose_state goose_append_default_to_chunk(goose_appender appender, goose_data_chunk chunk, idx_t col, idx_t row) {
	if (!appender || !chunk) {
		return GooseError;
	}

	auto *appender_instance = reinterpret_cast<AppenderWrapper *>(appender);

	auto data_chunk = reinterpret_cast<goose::DataChunk *>(chunk);

	try {
		appender_instance->appender->AppendDefault(*data_chunk, col, row);
	} catch (std::exception &ex) {
		appender_instance->error_data = ErrorData(ex);
		return GooseError;
	} catch (...) {
		return GooseError;
	}
	return GooseSuccess;
}

goose_state goose_append_bool(goose_appender appender, bool value) {
	return goose_append_internal<bool>(appender, value);
}

goose_state goose_append_int8(goose_appender appender, int8_t value) {
	return goose_append_internal<int8_t>(appender, value);
}

goose_state goose_append_int16(goose_appender appender, int16_t value) {
	return goose_append_internal<int16_t>(appender, value);
}

goose_state goose_append_int32(goose_appender appender, int32_t value) {
	return goose_append_internal<int32_t>(appender, value);
}

goose_state goose_append_int64(goose_appender appender, int64_t value) {
	return goose_append_internal<int64_t>(appender, value);
}

goose_state goose_append_hugeint(goose_appender appender, goose_hugeint value) {
	hugeint_t internal;
	internal.lower = value.lower;
	internal.upper = value.upper;
	return goose_append_internal<hugeint_t>(appender, internal);
}

goose_state goose_append_uint8(goose_appender appender, uint8_t value) {
	return goose_append_internal<uint8_t>(appender, value);
}

goose_state goose_append_uint16(goose_appender appender, uint16_t value) {
	return goose_append_internal<uint16_t>(appender, value);
}

goose_state goose_append_uint32(goose_appender appender, uint32_t value) {
	return goose_append_internal<uint32_t>(appender, value);
}

goose_state goose_append_uint64(goose_appender appender, uint64_t value) {
	return goose_append_internal<uint64_t>(appender, value);
}

goose_state goose_append_uhugeint(goose_appender appender, goose_uhugeint value) {
	uhugeint_t internal;
	internal.lower = value.lower;
	internal.upper = value.upper;
	return goose_append_internal<uhugeint_t>(appender, internal);
}

goose_state goose_append_float(goose_appender appender, float value) {
	return goose_append_internal<float>(appender, value);
}

goose_state goose_append_double(goose_appender appender, double value) {
	return goose_append_internal<double>(appender, value);
}

goose_state goose_append_date(goose_appender appender, goose_date value) {
	return goose_append_internal<date_t>(appender, date_t(value.days));
}

goose_state goose_append_time(goose_appender appender, goose_time value) {
	return goose_append_internal<dtime_t>(appender, dtime_t(value.micros));
}

goose_state goose_append_timestamp(goose_appender appender, goose_timestamp value) {
	return goose_append_internal<timestamp_t>(appender, timestamp_t(value.micros));
}

goose_state goose_append_interval(goose_appender appender, goose_interval value) {
	interval_t interval;
	interval.months = value.months;
	interval.days = value.days;
	interval.micros = value.micros;
	return goose_append_internal<interval_t>(appender, interval);
}

goose_state goose_append_null(goose_appender appender) {
	return goose_append_internal<std::nullptr_t>(appender, nullptr);
}

goose_state goose_append_varchar(goose_appender appender, const char *val) {
	return goose_append_internal<const char *>(appender, val);
}

goose_state goose_append_varchar_length(goose_appender appender, const char *val, idx_t length) {
	return goose_append_internal<string_t>(appender, string_t(val, goose::UnsafeNumericCast<uint32_t>(length)));
}

goose_state goose_append_blob(goose_appender appender, const void *data, idx_t length) {
	auto value = goose::Value::BLOB(goose::const_data_ptr_cast(data), length);
	return goose_append_internal<goose::Value>(appender, value);
}

goose_state goose_appender_flush(goose_appender appender_p) {
	return goose_appender_run_function(appender_p, [&](BaseAppender &appender) { appender.Flush(); });
}

goose_state goose_appender_clear(goose_appender appender_p) {
	return goose_appender_run_function(appender_p, [&](BaseAppender &appender) { appender.Clear(); });
}

goose_state goose_appender_close(goose_appender appender_p) {
	return goose_appender_run_function(appender_p, [&](BaseAppender &appender) { appender.Close(); });
}

idx_t goose_appender_column_count(goose_appender appender) {
	if (!appender) {
		return 0;
	}

	auto wrapper = reinterpret_cast<AppenderWrapper *>(appender);
	if (!wrapper->appender) {
		return 0;
	}

	return wrapper->appender->GetActiveTypes().size();
}

goose_logical_type goose_appender_column_type(goose_appender appender, idx_t col_idx) {
	if (!appender || col_idx >= goose_appender_column_count(appender)) {
		return nullptr;
	}

	auto wrapper = reinterpret_cast<AppenderWrapper *>(appender);
	if (!wrapper->appender) {
		return nullptr;
	}

	auto &logical_type = wrapper->appender->GetActiveTypes()[col_idx];
	return reinterpret_cast<goose_logical_type>(new goose::LogicalType(logical_type));
}

goose_state goose_append_value(goose_appender appender, goose_value value) {
	return goose_append_internal<goose::Value>(appender, *(reinterpret_cast<goose::Value *>(value)));
}

goose_state goose_append_data_chunk(goose_appender appender_p, goose_data_chunk chunk) {
	if (!chunk) {
		return GooseError;
	}
	auto data_chunk = reinterpret_cast<goose::DataChunk *>(chunk);
	return goose_appender_run_function(appender_p,
	                                    [&](BaseAppender &appender) { appender.AppendDataChunk(*data_chunk); });
}
