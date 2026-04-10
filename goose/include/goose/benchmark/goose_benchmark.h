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

#include <goose/benchmark/benchmark.h>
#include <goose/benchmark/benchmark_runner.h>
#include <goose/goose.h>
#include <goose/main/client_context.h>
#include <goose/testing/test_helpers.h>
#include <goose/main/query_profiler.h>
#include <goose/common/helper.h>

namespace goose {
    //! Base class for any state that has to be kept by a Benchmark
    struct GooseBenchmarkState : public BenchmarkState {
        Goose db;
        Connection conn;
        goose::unique_ptr<QueryResult> result;

        GooseBenchmarkState(string path) : db(path.empty() ? nullptr : path.c_str()), conn(db) {
            auto &instance = BenchmarkRunner::GetInstance();
            auto res = conn.Query("PRAGMA threads=" + to_string(instance.threads));
            if (!instance.memory_limit.empty()) {
                res = conn.Query("PRAGMA memory_limit='" + instance.memory_limit + "'");
                D_ASSERT(!res->HasError());
            }
            D_ASSERT(!res->HasError());
            string profiling_mode;
            switch (instance.configuration.profile_info) {
                case BenchmarkProfileInfo::NONE:
                    profiling_mode = "";
                    break;
                case BenchmarkProfileInfo::NORMAL:
                    profiling_mode = "standard";
                    break;
                case BenchmarkProfileInfo::DETAILED:
                    profiling_mode = "detailed";
                    break;
                default:
                    throw InternalException("Unknown profiling option \"%s\"", instance.configuration.profile_info);
            }
            if (!profiling_mode.empty()) {
                res = conn.Query("PRAGMA profiling_mode=" + profiling_mode);
                D_ASSERT(!res->HasError());
            }
        }

        virtual ~GooseBenchmarkState() {
        }
    };

    //! The base Benchmark class is a base class that is used to create and register
    //! new benchmarks
    class GooseBenchmark : public Benchmark {
    public:
        GooseBenchmark(bool register_benchmark, string name, string group) : Benchmark(
            register_benchmark, name, group) {
        }

        virtual ~GooseBenchmark() {
        }

        //! Load data into Goose
        virtual void Load(GooseBenchmarkState *state) = 0;

        //! Run a bunch of queries, only called if GetQuery() returns an empty string
        virtual void RunBenchmark(GooseBenchmarkState *state) {
        }

        //! This function gets called after the GetQuery() method
        virtual void Cleanup(GooseBenchmarkState *state) {
        };

        //! Verify a result
        virtual string VerifyResult(QueryResult *result) = 0;

        //! Whether or not the benchmark is performed on an in-memory database
        virtual bool InMemory() {
            return true;
        }

        string GetDatabasePath() {
            if (!InMemory()) {
                string path = "goose_benchmark_db.db";
                DeleteDatabase(path);
                return path;
            } else {
                return string();
            }
        }

        virtual goose::unique_ptr<GooseBenchmarkState> CreateBenchmarkState() {
            return make_uniq<GooseBenchmarkState>(GetDatabasePath());
        }

        goose::unique_ptr<BenchmarkState> Initialize(BenchmarkConfiguration &config) override {
            auto state = CreateBenchmarkState();
            Load(state.get());
            return std::move(state);
        }

        void Run(BenchmarkState *state_p) override {
            auto state = (GooseBenchmarkState *) state_p;
            string query = GetQuery();
            if (query.empty()) {
                RunBenchmark(state);
            } else {
                state->result = state->conn.Query(query);
            }
        }

        void Cleanup(BenchmarkState *state_p) override {
            auto state = (GooseBenchmarkState *) state_p;
            Cleanup(state);
        }

        string Verify(BenchmarkState *state_p) override {
            auto state = (GooseBenchmarkState *) state_p;
            return VerifyResult(state->result.get());
        }

        string GetLogOutput(BenchmarkState *state_p) override {
            auto state = (GooseBenchmarkState *) state_p;
            auto &profiler = QueryProfiler::Get(*state->conn.context);
            return profiler.ToJSON();
        }

        //! Interrupt the benchmark because of a timeout
        void Interrupt(BenchmarkState *state_p) override {
            auto state = (GooseBenchmarkState *) state_p;
            state->conn.Interrupt();
        }
    };

    void set_benchmark_directory(const std::string &test_directory);

    const std::string &get_benchmark_directory();

    void init_goose_benchmark();
} // namespace goose
