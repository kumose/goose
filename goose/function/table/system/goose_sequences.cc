#include <goose/function/table/system_functions.h>

#include <goose/catalog/catalog.h>
#include <goose/catalog/catalog_entry/schema_catalog_entry.h>
#include <goose/catalog/catalog_entry/sequence_catalog_entry.h>
#include <goose/common/exception.h>
#include <goose/common/numeric_utils.h>
#include <goose/main/client_context.h>
#include <goose/main/client_data.h>

namespace goose {

struct GooseSequencesData : public GlobalTableFunctionState {
	GooseSequencesData() : offset(0) {
	}

	vector<reference<SequenceCatalogEntry>> entries;
	idx_t offset;
};

static unique_ptr<FunctionData> GooseSequencesBind(ClientContext &context, TableFunctionBindInput &input,
                                                    vector<LogicalType> &return_types, vector<string> &names) {
	names.emplace_back("database_name");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("database_oid");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("schema_name");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("schema_oid");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("sequence_name");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("sequence_oid");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("comment");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("tags");
	return_types.emplace_back(LogicalType::MAP(LogicalType::VARCHAR, LogicalType::VARCHAR));

	names.emplace_back("temporary");
	return_types.emplace_back(LogicalType::BOOLEAN);

	names.emplace_back("start_value");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("min_value");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("max_value");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("increment_by");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("cycle");
	return_types.emplace_back(LogicalType::BOOLEAN);

	names.emplace_back("last_value");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("sql");
	return_types.emplace_back(LogicalType::VARCHAR);

	return nullptr;
}

unique_ptr<GlobalTableFunctionState> GooseSequencesInit(ClientContext &context, TableFunctionInitInput &input) {
	auto result = make_uniq<GooseSequencesData>();

	// scan all the schemas for tables and collect themand collect them
	auto schemas = Catalog::GetAllSchemas(context);
	for (auto &schema : schemas) {
		schema.get().Scan(context, CatalogType::SEQUENCE_ENTRY,
		                  [&](CatalogEntry &entry) { result->entries.push_back(entry.Cast<SequenceCatalogEntry>()); });
	};
	return result;
}

void GooseSequencesFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
	auto &data = data_p.global_state->Cast<GooseSequencesData>();
	if (data.offset >= data.entries.size()) {
		// finished returning values
		return;
	}
	// start returning values
	// either fill up the chunk or return all the remaining columns
	idx_t count = 0;
	while (data.offset < data.entries.size() && count < STANDARD_VECTOR_SIZE) {
		auto &seq = data.entries[data.offset++].get();
		auto seq_data = seq.GetData();

		// return values:
		idx_t col = 0;
		// database_name, VARCHAR
		output.SetValue(col++, count, seq.catalog.GetName());
		// database_oid, BIGINT
		output.SetValue(col++, count, Value::BIGINT(NumericCast<int64_t>(seq.catalog.GetOid())));
		// schema_name, VARCHAR
		output.SetValue(col++, count, Value(seq.schema.name));
		// schema_oid, BIGINT
		output.SetValue(col++, count, Value::BIGINT(NumericCast<int64_t>(seq.schema.oid)));
		// sequence_name, VARCHAR
		output.SetValue(col++, count, Value(seq.name));
		// sequence_oid, BIGINT
		output.SetValue(col++, count, Value::BIGINT(NumericCast<int64_t>(seq.oid)));
		// comment, VARCHAR
		output.SetValue(col++, count, Value(seq.comment));
		// tags, MAP(VARCHAR, VARCHAR)
		output.SetValue(col++, count, Value::MAP(seq.tags));
		// temporary, BOOLEAN
		output.SetValue(col++, count, Value::BOOLEAN(seq.temporary));
		// start_value, BIGINT
		output.SetValue(col++, count, Value::BIGINT(seq_data.start_value));
		// min_value, BIGINT
		output.SetValue(col++, count, Value::BIGINT(seq_data.min_value));
		// max_value, BIGINT
		output.SetValue(col++, count, Value::BIGINT(seq_data.max_value));
		// increment_by, BIGINT
		output.SetValue(col++, count, Value::BIGINT(seq_data.increment));
		// cycle, BOOLEAN
		output.SetValue(col++, count, Value::BOOLEAN(seq_data.cycle));
		// last_value, BIGINT
		output.SetValue(col++, count, seq_data.usage_count == 0 ? Value() : Value::BIGINT(seq_data.last_value));
		// sql, LogicalType::VARCHAR
		output.SetValue(col++, count, Value(seq.ToSQL()));

		count++;
	}
	output.SetCardinality(count);
}

void GooseSequencesFun::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(
	    TableFunction("goose_sequences", {}, GooseSequencesFunction, GooseSequencesBind, GooseSequencesInit));
}

} // namespace goose
