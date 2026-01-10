#include <goose/function/table/system_functions.h>

#include <goose/catalog/catalog.h>
#include <goose/catalog/catalog_entry/table_catalog_entry.h>
#include <goose/catalog/catalog_entry/view_catalog_entry.h>
#include <goose/parser/qualified_name.h>
#include <goose/parser/constraints/not_null_constraint.h>
#include <goose/parser/constraints/unique_constraint.h>
#include <goose/planner/expression/bound_parameter_expression.h>
#include <goose/planner/binder.h>

#include <goose/common/exception.h>
#include <goose/common/limits.h>

#include <algorithm>

namespace goose {

struct GooseTableSampleFunctionData : public TableFunctionData {
	explicit GooseTableSampleFunctionData(CatalogEntry &entry_p) : entry(entry_p) {
	}
	CatalogEntry &entry;
};

struct GooseTableSampleOperatorData : public GlobalTableFunctionState {
	GooseTableSampleOperatorData() : sample_offset(0) {
		sample = nullptr;
	}
	idx_t sample_offset;
	unique_ptr<BlockingSample> sample;
};

static unique_ptr<FunctionData> GooseTableSampleBind(ClientContext &context, TableFunctionBindInput &input,
                                                      vector<LogicalType> &return_types, vector<string> &names) {
	// look up the table name in the catalog
	auto qname = QualifiedName::Parse(input.inputs[0].GetValue<string>());
	Binder::BindSchemaOrCatalog(context, qname.catalog, qname.schema);

	auto &entry = Catalog::GetEntry<TableCatalogEntry>(context, qname.catalog, qname.schema, qname.name);
	if (entry.type != CatalogType::TABLE_ENTRY) {
		throw NotImplementedException("Invalid Catalog type passed to table_sample()");
	}
	auto &table_entry = entry.Cast<TableCatalogEntry>();
	auto types = table_entry.GetTypes();
	for (auto &type : types) {
		return_types.push_back(type);
	}
	for (idx_t i = 0; i < types.size(); i++) {
		auto logical_index = LogicalIndex(i);
		auto &col = table_entry.GetColumn(logical_index);
		names.push_back(col.GetName());
	}

	return make_uniq<GooseTableSampleFunctionData>(entry);
}

unique_ptr<GlobalTableFunctionState> GooseTableSampleInit(ClientContext &context, TableFunctionInitInput &input) {
	return make_uniq<GooseTableSampleOperatorData>();
}

static void GooseTableSampleTable(ClientContext &context, GooseTableSampleOperatorData &data,
                                   TableCatalogEntry &table, DataChunk &output) {
	// if table has statistics.
	// copy the sample of statistics into the output chunk
	if (!data.sample) {
		data.sample = table.GetSample();
	}
	if (data.sample) {
		auto sample_chunk = data.sample->GetChunk();
		if (sample_chunk) {
			sample_chunk->Copy(output, 0);
			data.sample_offset += sample_chunk->size();
		}
	}
}

static void GooseTableSampleFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
	auto &bind_data = data_p.bind_data->Cast<GooseTableSampleFunctionData>();
	auto &state = data_p.global_state->Cast<GooseTableSampleOperatorData>();
	switch (bind_data.entry.type) {
	case CatalogType::TABLE_ENTRY:
		GooseTableSampleTable(context, state, bind_data.entry.Cast<TableCatalogEntry>(), output);
		break;
	default:
		throw NotImplementedException("Unimplemented catalog type for pragma_table_sample");
	}
}

void GooseTableSample::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(TableFunction("goose_table_sample", {LogicalType::VARCHAR}, GooseTableSampleFunction,
	                              GooseTableSampleBind, GooseTableSampleInit));
}

} // namespace goose
