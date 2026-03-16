// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "benchmark.hpp"
#include <goose/main/query_result.h>

#include <unordered_map>
#include <unordered_set>

namespace goose {
struct BenchmarkFileReader;
class MaterializedQueryResult;
struct InterpretedBenchmarkState;

const string DEFAULT_DB_PATH = "goose_benchmark_db.db";

struct BenchmarkQuery {
public:
	BenchmarkQuery() {
	}

public:
	string query;
	idx_t column_count = 0;
	vector<vector<string>> expected_result;
};

//! Interpreted benchmarks read the benchmark from a file
class InterpretedBenchmark : public Benchmark {
public:
	InterpretedBenchmark(string full_path);

	void LoadBenchmark();
	//! Initialize the benchmark state
	goose::unique_ptr<BenchmarkState> Initialize(BenchmarkConfiguration &config) override;
	//! Assert correct/expected state of the db, before Run
	void Assert(BenchmarkState *state) override;
	//! Run the benchmark
	void Run(BenchmarkState *state) override;
	//! Cleanup the benchmark, called after each Run
	void Cleanup(BenchmarkState *state) override;
	//! Verify that the output of the benchmark was correct
	string Verify(BenchmarkState *state) override;

	string GetQuery() override;
	//! Interrupt the benchmark because of a timeout
	void Interrupt(BenchmarkState *state) override;
	//! Returns information about the benchmark
	string BenchmarkInfo() override;

	string GetLogOutput(BenchmarkState *state) override;

	string DisplayName() override;
	string Group() override;
	string Subgroup() override;

	string GetDatabasePath();

	bool InMemory() {
		return in_memory;
	}

	bool RequireReinit() override {
		return require_reinit;
	}
	QueryResultType ResultMode() const {
		return result_type;
	}
	idx_t ArrowBatchSize() const {
		return arrow_batch_size;
	}

private:
	string VerifyInternal(BenchmarkState *state_p, const BenchmarkQuery &query, MaterializedQueryResult &result);

	BenchmarkQuery ReadQueryFromFile(BenchmarkFileReader &reader, string file);
	BenchmarkQuery ReadQueryFromReader(BenchmarkFileReader &reader, const string &sql, const string &header);

	unique_ptr<QueryResult> RunLoadQuery(InterpretedBenchmarkState &state, const string &load_query);

	void ProcessFile(const string &path);

private:
	bool is_loaded = false;
	std::unordered_map<string, string> replacement_mapping;
	unordered_set<string> handled_arguments;

	std::unordered_map<string, string> queries;
	string run_query;

	string benchmark_path;
	string cache_db = "";
	string cache_file = "";
	// check the existence of a cached db, but do not connect
	// can be used to test accessing data from a different db in a non-persistent connection
	bool cache_no_connect = false;
	std::unordered_set<string> extensions;
	std::unordered_set<string> load_extensions;

	//! Queries used to assert a given state of the data
	vector<BenchmarkQuery> assert_queries;
	vector<BenchmarkQuery> result_queries;
	//! How many times to retry the load, if any
	idx_t retry_load = 0;

	string display_name;
	string display_group;
	string subgroup;

	bool in_memory = true;
	string storage_version;
	QueryResultType result_type = QueryResultType::MATERIALIZED_RESULT;
	idx_t arrow_batch_size = STANDARD_VECTOR_SIZE;
	bool require_reinit = false;
};

} // namespace goose
