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

#include <goose/benchmark/benchmark_configuration.h>
#include <goose/benchmark/benchmark.h>
#include <goose/common/constants.h>
#include <goose/common/types-import.h>
#include <thread>

namespace goose {
    class Goose;

    //! The benchmark runner class is responsible for running benchmarks
    class BenchmarkRunner {
        BenchmarkRunner();

    public:
        static constexpr const char *GOOSE_BENCHMARK_DIRECTORY = "goose_benchmark_data";
        BenchmarkConfiguration configuration;

        static BenchmarkRunner &GetInstance() {
            static BenchmarkRunner instance;
            return instance;
        }

        static void InitializeBenchmarkDirectory();

        //! Register a benchmark in the Benchmark Runner, this is done automatically
        //! as long as the proper macro's are used
        static void RegisterBenchmark(Benchmark *benchmark);

        void Log(string message);

        void LogLine(string message);

        void LogResult(string message);

        void LogOutput(string message);

        void LogSummary(string benchmark, string message, size_t i);

        void RunBenchmark(Benchmark *benchmark);

        void RunBenchmarks();

        vector<Benchmark *> benchmarks;
        ofstream out_file;
        ofstream log_file;
        uint32_t threads = MaxValue<uint32_t>(std::thread::hardware_concurrency(), 1u);
        string memory_limit;
        unordered_map<string, string> custom_arguments;
    };
} // namespace goose
