#include <goose/common/string_util.h>
#include <goose/main/capi/capi_internal.h>

using goose::Connection;
using goose::ErrorData;
using goose::TableDescription;
using goose::TableDescriptionWrapper;

goose_state goose_table_description_create(goose_connection connection, const char *schema, const char *table,
                                             goose_table_description *out) {
	return goose_table_description_create_ext(connection, INVALID_CATALOG, schema, table, out);
}

goose_state goose_table_description_create_ext(goose_connection connection, const char *catalog, const char *schema,
                                                 const char *table, goose_table_description *out) {
	Connection *conn = reinterpret_cast<Connection *>(connection);

	if (!out) {
		return GooseError;
	}
	auto wrapper = new TableDescriptionWrapper();
	*out = reinterpret_cast<goose_table_description>(wrapper);

	if (!connection || !table) {
		return GooseError;
	}
	if (catalog == nullptr) {
		catalog = INVALID_CATALOG;
	}
	if (schema == nullptr) {
		schema = DEFAULT_SCHEMA;
	}

	try {
		wrapper->description = conn->TableInfo(catalog, schema, table);
	} catch (std::exception &ex) {
		ErrorData error(ex);
		wrapper->error = error.RawMessage();
		return GooseError;
	} catch (...) { // LCOV_EXCL_START
		wrapper->error = "Unknown Connection::TableInfo error";
		return GooseError;
	} // LCOV_EXCL_STOP
	if (!wrapper->description) {
		wrapper->error = "No table with that schema+name could be located";
		return GooseError;
	}
	return GooseSuccess;
}

void goose_table_description_destroy(goose_table_description *table) {
	if (!table || !*table) {
		return;
	}
	auto wrapper = reinterpret_cast<TableDescriptionWrapper *>(*table);
	delete wrapper;
	*table = nullptr;
}

const char *goose_table_description_error(goose_table_description table) {
	if (!table) {
		return nullptr;
	}
	auto wrapper = reinterpret_cast<TableDescriptionWrapper *>(table);
	if (wrapper->error.empty()) {
		return nullptr;
	}
	return wrapper->error.c_str();
}

goose_state GetTableDescription(TableDescriptionWrapper *wrapper, goose::optional_idx index) {
	if (!wrapper) {
		return GooseError;
	}
	auto &table = wrapper->description;
	if (index.IsValid() && index.GetIndex() >= table->columns.size()) {
		wrapper->error = goose::StringUtil::Format("Column index %d is out of range, table only has %d columns",
		                                            index.GetIndex(), table->columns.size());
		return GooseError;
	}
	return GooseSuccess;
}

goose_state goose_column_has_default(goose_table_description table_description, idx_t index, bool *out) {
	auto wrapper = reinterpret_cast<TableDescriptionWrapper *>(table_description);
	if (GetTableDescription(wrapper, index) == GooseError) {
		return GooseError;
	}
	if (!out) {
		wrapper->error = "Please provide a valid (non-null) 'out' variable";
		return GooseError;
	}

	auto &table = wrapper->description;
	auto &column = table->columns[index];
	*out = column.HasDefaultValue();
	return GooseSuccess;
}

idx_t goose_table_description_get_column_count(goose_table_description table_description) {
	auto wrapper = reinterpret_cast<TableDescriptionWrapper *>(table_description);
	if (GetTableDescription(wrapper, goose::optional_idx()) == GooseError) {
		return 0;
	}

	auto &table = wrapper->description;
	return table->columns.size();
}

char *goose_table_description_get_column_name(goose_table_description table_description, idx_t index) {
	auto wrapper = reinterpret_cast<TableDescriptionWrapper *>(table_description);
	if (GetTableDescription(wrapper, index) == GooseError) {
		return nullptr;
	}

	auto &table = wrapper->description;
	auto &column = table->columns[index];

	auto name = column.GetName();
	auto result = reinterpret_cast<char *>(malloc(sizeof(char) * (name.size() + 1)));
	memcpy(result, name.c_str(), name.size());
	result[name.size()] = '\0';

	return result;
}

goose_logical_type goose_table_description_get_column_type(goose_table_description table_description, idx_t index) {
	auto wrapper = reinterpret_cast<TableDescriptionWrapper *>(table_description);
	if (GetTableDescription(wrapper, index) == GooseError) {
		return nullptr;
	}

	auto &table = wrapper->description;
	auto &column = table->columns[index];

	auto logical_type = new goose::LogicalType(column.Type());
	return reinterpret_cast<goose_logical_type>(logical_type);
}
