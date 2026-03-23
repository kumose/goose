#include <goose/function/table/range.h>
#include <goose/main/client_context.h>
#include <goose/storage/storage_manager.h>
#include <goose/transaction/transaction_manager.h>
#include <goose/main/database_manager.h>
#include <goose/function/function_set.h>

namespace goose {

struct CheckpointBindData : public FunctionData {
	explicit CheckpointBindData(optional_ptr<AttachedDatabase> db) : db(db) {
	}

	optional_ptr<AttachedDatabase> db;

public:
	unique_ptr<FunctionData> Copy() const override {
		return make_uniq<CheckpointBindData>(db);
	}

	bool Equals(const FunctionData &other_p) const override {
		auto &other = other_p.Cast<CheckpointBindData>();
		return db == other.db;
	}
};

static unique_ptr<FunctionData> CheckpointBind(ClientContext &context, TableFunctionBindInput &input,
                                               vector<LogicalType> &return_types, vector<string> &names) {
	return_types.emplace_back(LogicalType::BOOLEAN);
	names.emplace_back("Success");

	optional_ptr<AttachedDatabase> db;
	auto &db_manager = DatabaseManager::Get(context);
	if (!input.inputs.empty()) {
		if (input.inputs[0].IsNull()) {
			throw BinderException("Database cannot be NULL");
		}
		auto &db_name = StringValue::Get(input.inputs[0]);
		db = db_manager.GetDatabase(context, db_name);
		if (!db) {
			throw BinderException("Database \"%s\" not found", db_name);
		}
	} else {
		db = db_manager.GetDatabase(context, DatabaseManager::GetDefaultDatabase(context));
	}
	return make_uniq<CheckpointBindData>(db);
}

template <bool FORCE>
static void TemplatedCheckpointFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
	auto &bind_data = data_p.bind_data->Cast<CheckpointBindData>();
	auto &transaction_manager = TransactionManager::Get(*bind_data.db.get_mutable());
	transaction_manager.Checkpoint(context, FORCE);
}

void CheckpointFunction::RegisterFunction(BuiltinFunctions &set) {
	TableFunctionSet checkpoint("checkpoint");
	checkpoint.AddFunction(TableFunction({}, TemplatedCheckpointFunction<false>, CheckpointBind));
	checkpoint.AddFunction(TableFunction({LogicalType::VARCHAR}, TemplatedCheckpointFunction<false>, CheckpointBind));
	set.AddFunction(checkpoint);

	TableFunctionSet force_checkpoint("force_checkpoint");
	force_checkpoint.AddFunction(TableFunction({}, TemplatedCheckpointFunction<true>, CheckpointBind));
	force_checkpoint.AddFunction(
	    TableFunction({LogicalType::VARCHAR}, TemplatedCheckpointFunction<true>, CheckpointBind));
	set.AddFunction(force_checkpoint);
}

} // namespace goose
