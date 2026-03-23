#include <goose/common/arrow/arrow.h>
#include <goose/common/arrow/arrow_converter.h>
#include <goose/function/table/arrow.h>
#include <goose/main/capi/capi_internal.h>
#include <goose/main/prepared_statement_data.h>
#include <goose/utility/fmt/format.h>

using goose::ArrowConverter;
using goose::ArrowResultWrapper;
using goose::CClientArrowOptionsWrapper;
using goose::Connection;
using goose::DataChunk;
using goose::LogicalType;
using goose::MaterializedQueryResult;
using goose::PreparedStatementWrapper;
using goose::QueryResult;
using goose::QueryResultType;

goose_error_data goose_to_arrow_schema(goose_arrow_options arrow_options, goose_logical_type *types,
                                         const char **names, idx_t column_count, struct ArrowSchema *out_schema) {
	if (!arrow_options || !out_schema) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, "Invalid argument(s) to goose_to_arrow_schema");
	}
	// types and names can be nullptr when column_count is 0
	if (column_count > 0 && (!types || !names)) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, "Invalid argument(s) to goose_to_arrow_schema");
	}

	goose::vector<LogicalType> schema_types;
	goose::vector<std::string> schema_names;
	for (idx_t i = 0; i < column_count; i++) {
		schema_names.emplace_back(names[i]);
		schema_types.emplace_back(*reinterpret_cast<goose::LogicalType *>(types[i]));
	}
	const auto arrow_options_wrapper = reinterpret_cast<CClientArrowOptionsWrapper *>(arrow_options);
	try {
		ArrowConverter::ToArrowSchema(out_schema, schema_types, schema_names, arrow_options_wrapper->properties);
	} catch (const goose::Exception &ex) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, ex.what());
	} catch (const std::exception &ex) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, ex.what());
	} catch (...) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, "Unknown error occurred during conversion");
	}
	return nullptr;
}

goose_error_data goose_data_chunk_to_arrow(goose_arrow_options arrow_options, goose_data_chunk chunk,
                                             struct ArrowArray *out_arrow_array) {
	if (!arrow_options || !chunk || !out_arrow_array) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT,
		                                "Invalid argument(s) to goose_data_chunk_to_arrow");
	}
	auto dchunk = reinterpret_cast<goose::DataChunk *>(chunk);
	auto arrow_options_wrapper = reinterpret_cast<CClientArrowOptionsWrapper *>(arrow_options);
	auto extension_type_cast = goose::ArrowTypeExtensionData::GetExtensionTypes(
	    *arrow_options_wrapper->properties.client_context, dchunk->GetTypes());

	try {
		ArrowConverter::ToArrowArray(*dchunk, out_arrow_array, arrow_options_wrapper->properties, extension_type_cast);
	} catch (const goose::Exception &ex) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, ex.what());
	} catch (const std::exception &ex) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, ex.what());
	} catch (...) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, "Unknown error occurred during conversion");
	}
	return nullptr;
}

goose_error_data goose_schema_from_arrow(goose_connection connection, struct ArrowSchema *schema,
                                           goose_arrow_converted_schema *out_types) {
	if (!connection || !out_types || !schema) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT,
		                                "Invalid argument(s) to goose_data_chunk_to_arrow");
	}
	goose::vector<std::string> names;
	const auto conn = reinterpret_cast<Connection *>(connection);
	auto arrow_table = goose::make_uniq<goose::ArrowTableSchema>();
	try {
		goose::vector<LogicalType> return_types;
		goose::ArrowTableFunction::PopulateArrowTableSchema(goose::DBConfig::GetConfig(*conn->context), *arrow_table,
		                                                     *schema);
	} catch (const goose::Exception &ex) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, ex.what());
	} catch (const std::exception &ex) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, ex.what());
	} catch (...) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, "Unknown error occurred during conversion");
	}
	*out_types = reinterpret_cast<goose_arrow_converted_schema>(arrow_table.release());
	return nullptr;
}

goose_error_data goose_data_chunk_from_arrow(goose_connection connection, struct ArrowArray *arrow_array,
                                               goose_arrow_converted_schema converted_schema,
                                               goose_data_chunk *out_chunk) {
	if (!connection || !converted_schema || !out_chunk || !arrow_array) {
		return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT,
		                                "Invalid argument(s) to goose_data_chunk_to_arrow");
	}
	auto arrow_table = reinterpret_cast<goose::ArrowTableSchema *>(converted_schema);
	auto conn = reinterpret_cast<Connection *>(connection);
	auto &types = arrow_table->GetTypes();

	auto dchunk = goose::make_uniq<goose::DataChunk>();
	dchunk->Initialize(goose::Allocator::DefaultAllocator(), types, goose::NumericCast<idx_t>(arrow_array->length));

	auto &arrow_types = arrow_table->GetColumns();
	dchunk->SetCardinality(goose::NumericCast<idx_t>(arrow_array->length));
	for (idx_t i = 0; i < dchunk->ColumnCount(); i++) {
		auto &parent_array = *arrow_array;
		auto &array = parent_array.children[i];
		auto arrow_type = arrow_types.at(i);
		auto array_physical_type = arrow_type->GetPhysicalType();
		auto array_state = goose::make_uniq<goose::ArrowArrayScanState>(*conn->context);
		// We need to make sure that our chunk will hold the ownership
		array_state->owned_data = goose::make_shared_ptr<goose::ArrowArrayWrapper>();
		array_state->owned_data->arrow_array = *arrow_array;
		// We set it to nullptr to effectively transfer the ownership
		arrow_array->release = nullptr;
		try {
			switch (array_physical_type) {
			case goose::ArrowArrayPhysicalType::DICTIONARY_ENCODED:
				goose::ArrowToGooseConversion::ColumnArrowToGooseDictionary(dchunk->data[i], *array, 0, *array_state,
				                                                               dchunk->size(), *arrow_type);
				break;
			case goose::ArrowArrayPhysicalType::RUN_END_ENCODED:
				goose::ArrowToGooseConversion::ColumnArrowToGooseRunEndEncoded(
				    dchunk->data[i], *array, 0, *array_state, dchunk->size(), *arrow_type);
				break;
			case goose::ArrowArrayPhysicalType::DEFAULT:
				goose::ArrowToGooseConversion::SetValidityMask(dchunk->data[i], *array, 0, dchunk->size(),
				                                                 parent_array.offset, -1);

				goose::ArrowToGooseConversion::ColumnArrowToGoose(dchunk->data[i], *array, 0, *array_state,
				                                                     dchunk->size(), *arrow_type);
				break;
			default:
				return goose_create_error_data(GOOSE_ERROR_NOT_IMPLEMENTED,
				                                "Only Default Physical Types are currently supported");
			}
		} catch (const goose::Exception &ex) {
			return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, ex.what());
		} catch (const std::exception &ex) {
			return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, ex.what());
		} catch (...) {
			return goose_create_error_data(GOOSE_ERROR_INVALID_INPUT, "Unknown error occurred during conversion");
		}
	}
	*out_chunk = reinterpret_cast<goose_data_chunk>(dchunk.release());
	return nullptr;
}

void goose_destroy_arrow_converted_schema(goose_arrow_converted_schema *arrow_converted_schema) {
	if (arrow_converted_schema && *arrow_converted_schema) {
		auto converted_schema = reinterpret_cast<goose::ArrowTableSchema *>(*arrow_converted_schema);
		delete converted_schema;
		*arrow_converted_schema = nullptr;
	}
}

goose_state goose_query_arrow(goose_connection connection, const char *query, goose_arrow *out_result) {
	Connection *conn = (Connection *)connection;
	auto wrapper = new ArrowResultWrapper();
	try {
		wrapper->result = conn->Query(query);
		*out_result = (goose_arrow)wrapper;
		return !wrapper->result->HasError() ? GooseSuccess : GooseError;
	} catch (...) {
		delete wrapper;
		return GooseError;
	}
}

goose_state goose_query_arrow_schema(goose_arrow result, goose_arrow_schema *out_schema) {
	if (!out_schema) {
		return GooseSuccess;
	}
	auto wrapper = reinterpret_cast<ArrowResultWrapper *>(result);
	try {
		ArrowConverter::ToArrowSchema((ArrowSchema *)*out_schema, wrapper->result->types, wrapper->result->names,
		                              wrapper->result->client_properties);
	} catch (...) {
		return GooseError;
	}
	return GooseSuccess;
}

goose_state goose_prepared_arrow_schema(goose_prepared_statement prepared, goose_arrow_schema *out_schema) {
	if (!out_schema) {
		return GooseSuccess;
	}
	auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared);
	if (!wrapper || !wrapper->statement || !wrapper->statement->data) {
		return GooseError;
	}
	auto properties = wrapper->statement->context->GetClientProperties();
	goose::vector<goose::LogicalType> prepared_types;
	goose::vector<goose::string> prepared_names;

	auto count = wrapper->statement->data->properties.parameter_count;
	for (idx_t i = 0; i < count; i++) {
		// Every prepared parameter type is UNKNOWN, which we need to map to NULL according to the spec of
		// 'AdbcStatementGetParameterSchema'
		const auto type = LogicalType::SQLNULL;

		// FIXME: we don't support named parameters yet, but when we do, this needs to be updated
		auto name = std::to_string(i);
		prepared_types.push_back(type);
		prepared_names.push_back(name);
	}

	auto result_schema = (ArrowSchema *)*out_schema;
	if (!result_schema) {
		return GooseError;
	}

	if (result_schema->release) {
		// Need to release the existing schema before we overwrite it
		result_schema->release(result_schema);
		D_ASSERT(!result_schema->release);
	}

	ArrowConverter::ToArrowSchema(result_schema, prepared_types, prepared_names, properties);
	return GooseSuccess;
}

goose_state goose_query_arrow_array(goose_arrow result, goose_arrow_array *out_array) {
	if (!out_array) {
		return GooseSuccess;
	}
	auto wrapper = reinterpret_cast<ArrowResultWrapper *>(result);
	auto success = wrapper->result->TryFetch(wrapper->current_chunk, wrapper->result->GetErrorObject());
	if (!success) { // LCOV_EXCL_START
		return GooseError;
	} // LCOV_EXCL_STOP
	if (!wrapper->current_chunk || wrapper->current_chunk->size() == 0) {
		return GooseSuccess;
	}
	auto extension_type_cast = goose::ArrowTypeExtensionData::GetExtensionTypes(
	    *wrapper->result->client_properties.client_context, wrapper->result->types);
	ArrowConverter::ToArrowArray(*wrapper->current_chunk, reinterpret_cast<ArrowArray *>(*out_array),
	                             wrapper->result->client_properties, extension_type_cast);
	return GooseSuccess;
}

void goose_result_arrow_array(goose_result result, goose_data_chunk chunk, goose_arrow_array *out_array) {
	if (!out_array) {
		return;
	}
	auto dchunk = reinterpret_cast<goose::DataChunk *>(chunk);
	auto &result_data = *(reinterpret_cast<goose::GooseResultData *>(result.internal_data));
	auto extension_type_cast = goose::ArrowTypeExtensionData::GetExtensionTypes(
	    *result_data.result->client_properties.client_context, result_data.result->types);

	ArrowConverter::ToArrowArray(*dchunk, reinterpret_cast<ArrowArray *>(*out_array),
	                             result_data.result->client_properties, extension_type_cast);
}

idx_t goose_arrow_row_count(goose_arrow result) {
	auto wrapper = reinterpret_cast<ArrowResultWrapper *>(result);
	if (wrapper->result->HasError()) {
		return 0;
	}
	return wrapper->result->RowCount();
}

idx_t goose_arrow_column_count(goose_arrow result) {
	auto wrapper = reinterpret_cast<ArrowResultWrapper *>(result);
	return wrapper->result->ColumnCount();
}

idx_t goose_arrow_rows_changed(goose_arrow result) {
	auto wrapper = reinterpret_cast<ArrowResultWrapper *>(result);
	if (wrapper->result->HasError()) {
		return 0;
	}
	idx_t rows_changed = 0;
	auto &collection = wrapper->result->Collection();
	idx_t row_count = collection.Count();
	if (row_count > 0 && wrapper->result->properties.return_type == goose::StatementReturnType::CHANGED_ROWS) {
		auto rows = collection.GetRows();
		D_ASSERT(row_count == 1);
		D_ASSERT(rows.size() == 1);
		rows_changed = goose::NumericCast<idx_t>(rows[0].GetValue(0).GetValue<int64_t>());
	}
	return rows_changed;
}

const char *goose_query_arrow_error(goose_arrow result) {
	auto wrapper = reinterpret_cast<ArrowResultWrapper *>(result);
	return wrapper->result->GetError().c_str();
}

void goose_destroy_arrow(goose_arrow *result) {
	if (*result) {
		auto wrapper = reinterpret_cast<ArrowResultWrapper *>(*result);
		delete wrapper;
		*result = nullptr;
	}
}

void goose_destroy_arrow_stream(goose_arrow_stream *stream_p) {
	auto stream = reinterpret_cast<ArrowArrayStream *>(*stream_p);
	if (!stream) {
		return;
	}
	if (stream->release) {
		stream->release(stream);
	}
	D_ASSERT(!stream->release);

	delete stream;
	*stream_p = nullptr;
}

goose_state goose_execute_prepared_arrow(goose_prepared_statement prepared_statement, goose_arrow *out_result) {
	auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared_statement);
	if (!wrapper || !wrapper->statement || wrapper->statement->HasError() || !out_result) {
		return GooseError;
	}
	auto arrow_wrapper = new ArrowResultWrapper();
	try {
		auto result = wrapper->statement->Execute(wrapper->values, false);
		D_ASSERT(result->type == QueryResultType::MATERIALIZED_RESULT);
		arrow_wrapper->result = goose::unique_ptr_cast<QueryResult, MaterializedQueryResult>(std::move(result));
		*out_result = reinterpret_cast<goose_arrow>(arrow_wrapper);
		return !arrow_wrapper->result->HasError() ? GooseSuccess : GooseError;
	} catch (...) {
		delete arrow_wrapper;
		return GooseError;
	}
}

namespace arrow_array_stream_wrapper {
namespace {
struct PrivateData {
	ArrowSchema *schema;
	ArrowArray *array;
	bool done = false;
};

// LCOV_EXCL_START
// This function is never called, but used to set ArrowSchema's release functions to a non-null NOOP.
void EmptySchemaRelease(ArrowSchema *schema) {
	schema->release = nullptr;
}
// LCOV_EXCL_STOP

void EmptyArrayRelease(ArrowArray *array) {
	array->release = nullptr;
}

void EmptyStreamRelease(ArrowArrayStream *stream) {
	stream->release = nullptr;
}

void FactoryGetSchema(ArrowArrayStream *stream, ArrowSchema &schema) {
	stream->get_schema(stream, &schema);

	// Need to nullify the root schema's release function here, because streams don't allow us to set the release
	// function. For the schema's children, we nullify the release functions in `goose_arrow_scan`, so we don't need to
	// handle them again here. We set this to nullptr and not EmptySchemaRelease to prevent ArrowSchemaWrapper's
	// destructor from destroying the schema (it's the caller's responsibility).
	schema.release = nullptr;
}

int GetSchema(struct ArrowArrayStream *stream, struct ArrowSchema *out) {
	auto private_data = static_cast<arrow_array_stream_wrapper::PrivateData *>((stream->private_data));
	if (private_data->schema == nullptr) {
		return GooseError;
	}

	*out = *private_data->schema;
	out->release = EmptySchemaRelease;
	return GooseSuccess;
}

int GetNext(struct ArrowArrayStream *stream, struct ArrowArray *out) {
	auto private_data = static_cast<arrow_array_stream_wrapper::PrivateData *>((stream->private_data));
	*out = *private_data->array;
	if (private_data->done) {
		out->release = nullptr;
	} else {
		out->release = EmptyArrayRelease;
	}

	private_data->done = true;
	return GooseSuccess;
}

goose::unique_ptr<goose::ArrowArrayStreamWrapper> FactoryGetNext(uintptr_t stream_factory_ptr,
                                                                   goose::ArrowStreamParameters &parameters) {
	auto stream = reinterpret_cast<ArrowArrayStream *>(stream_factory_ptr);
	auto ret = goose::make_uniq<goose::ArrowArrayStreamWrapper>();
	ret->arrow_array_stream = *stream;
	ret->arrow_array_stream.release = EmptyStreamRelease;
	return ret;
}

// LCOV_EXCL_START
// This function is never be called, because it's used to construct a stream wrapping around a caller-supplied
// ArrowArray. Thus, the stream itself cannot produce an error.
const char *GetLastError(struct ArrowArrayStream *stream) {
	return nullptr;
}
// LCOV_EXCL_STOP

void Release(struct ArrowArrayStream *stream) {
	if (stream->private_data != nullptr) {
		delete reinterpret_cast<PrivateData *>(stream->private_data);
	}

	stream->private_data = nullptr;
	stream->release = nullptr;
}

goose_state Ingest(goose_connection connection, const char *table_name, struct ArrowArrayStream *input) {
	try {
		auto cconn = reinterpret_cast<goose::Connection *>(connection);
		cconn
		    ->TableFunction("arrow_scan", {goose::Value::POINTER((uintptr_t)input),
		                                   goose::Value::POINTER((uintptr_t)FactoryGetNext),
		                                   goose::Value::POINTER((uintptr_t)FactoryGetSchema)})
		    ->CreateView(table_name, true, false);
	} catch (...) { // LCOV_EXCL_START
		// Tried covering this in tests, but it proved harder than expected. At the time of writing:
		// - Passing any name to `CreateView` worked without throwing an exception
		// - Passing a null Arrow array worked without throwing an exception
		// - Passing an invalid schema (without any columns) led to an InternalException with SIGABRT, which is meant to
		//   be un-catchable. This case likely needs to be handled gracefully within `arrow_scan`.
		// Ref: https://discord.com/channels/909674491309850675/921100573732909107/1115230468699336785
		return GooseError;
	} // LCOV_EXCL_STOP

	return GooseSuccess;
}
} // namespace
} // namespace arrow_array_stream_wrapper

goose_state goose_arrow_scan(goose_connection connection, const char *table_name, goose_arrow_stream arrow) {
	auto stream = reinterpret_cast<ArrowArrayStream *>(arrow);

	// Backup release functions - we nullify children schema release functions because we don't want to release on
	// behalf of the caller, downstream in our code. Note that Arrow releases target immediate children, but aren't
	// recursive. So we only back up immediate children here and restore their functions.
	ArrowSchema schema;
	if (stream->get_schema(stream, &schema) == GooseError) {
		return GooseError;
	}

	typedef void (*release_fn_t)(ArrowSchema *);
	std::vector<release_fn_t> release_fns(goose::NumericCast<idx_t>(schema.n_children));
	for (idx_t i = 0; i < goose::NumericCast<idx_t>(schema.n_children); i++) {
		auto child = schema.children[i];
		release_fns[i] = child->release;
		child->release = arrow_array_stream_wrapper::EmptySchemaRelease;
	}

	auto ret = arrow_array_stream_wrapper::Ingest(connection, table_name, stream);

	// Restore release functions.
	for (idx_t i = 0; i < goose::NumericCast<idx_t>(schema.n_children); i++) {
		schema.children[i]->release = release_fns[i];
	}

	return ret;
}

goose_state goose_arrow_array_scan(goose_connection connection, const char *table_name,
                                     goose_arrow_schema arrow_schema, goose_arrow_array arrow_array,
                                     goose_arrow_stream *out_stream) {
	auto private_data = new arrow_array_stream_wrapper::PrivateData;
	private_data->schema = reinterpret_cast<ArrowSchema *>(arrow_schema);
	private_data->array = reinterpret_cast<ArrowArray *>(arrow_array);
	private_data->done = false;

	ArrowArrayStream *stream;
	try {
		stream = new ArrowArrayStream;
	} catch (...) {
		delete private_data;
		return GooseError;
	}
	try {
		*out_stream = reinterpret_cast<goose_arrow_stream>(stream);
		stream->get_schema = arrow_array_stream_wrapper::GetSchema;
		stream->get_next = arrow_array_stream_wrapper::GetNext;
		stream->get_last_error = arrow_array_stream_wrapper::GetLastError;
		stream->release = arrow_array_stream_wrapper::Release;
		stream->private_data = private_data;

		return goose_arrow_scan(connection, table_name, reinterpret_cast<goose_arrow_stream>(stream));
	} catch (...) {
		delete private_data;
		delete stream;
		return GooseError;
	}
}
