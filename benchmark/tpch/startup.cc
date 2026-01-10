#include "benchmark_runner.hpp"
#include "compare_result.h"
#include "goose_benchmark_macro.hpp"

using namespace goose;

#define SF 1

#define TPCHStartup(QUERY)                                                                                             \
	string db_path = "goose_benchmark_db.db";                                                                         \
	void Load(GooseBenchmarkState *state) override {                                                                  \
		DeleteDatabase(db_path);                                                                                       \
		{                                                                                                              \
			Goose db(db_path);                                                                                        \
			Connection con(db);                                                                                        \
			con.Query("CALL dbgen(sf=" + std::to_string(SF) + ")");                                                    \
		}                                                                                                              \
		{                                                                                                              \
			auto config = GetConfig();                                                                                 \
			config->options.checkpoint_wal_size = 0;                                                                   \
			Goose db(db_path, config.get());                                                                          \
		}                                                                                                              \
	}                                                                                                                  \
	void RunBenchmark(GooseBenchmarkState *state) override {                                                          \
		auto config = GetConfig();                                                                                     \
		Goose db(db_path, config.get());                                                                              \
		Connection con(db);                                                                                            \
		state->result = con.Query(QUERY);                                                                              \
	}                                                                                                                  \
	string BenchmarkInfo() override {                                                                                  \
		return string("Start a TPC-H SF1 database and run ") + QUERY + string(" in the database");                     \
	}

#define NormalConfig()                                                                                                 \
	goose::unique_ptr<DBConfig> GetConfig() {                                                                         \
		return make_uniq<DBConfig>();                                                                                  \
	}

GOOSE_BENCHMARK(TPCHEmptyStartup, "[startup]")
TPCHStartup("SELECT * FROM lineitem WHERE 1=0") NormalConfig() string VerifyResult(QueryResult *result) override {
	if (result->HasError()) {
		return result->GetError();
	}
	return string();
}
FINISH_BENCHMARK(TPCHEmptyStartup)

GOOSE_BENCHMARK(TPCHCount, "[startup]")
TPCHStartup("SELECT COUNT(*) FROM lineitem") NormalConfig() string VerifyResult(QueryResult *result) override {
	if (result->HasError()) {
		return result->GetError();
	}
	return string();
}
FINISH_BENCHMARK(TPCHCount)

GOOSE_BENCHMARK(TPCHSimpleAggr, "[startup]")
TPCHStartup("SELECT SUM(l_extendedprice) FROM lineitem") NormalConfig() string
    VerifyResult(QueryResult *result) override {
	if (result->HasError()) {
		return result->GetError();
	}
	return string();
}
FINISH_BENCHMARK(TPCHSimpleAggr)

GOOSE_BENCHMARK(TPCHQ1, "[startup]")
TPCHStartup("PRAGMA tpch(1)") NormalConfig() string VerifyResult(QueryResult *result) override {
	if (result->HasError()) {
		return result->GetError();
	}
	return string();
}
FINISH_BENCHMARK(TPCHQ1)
