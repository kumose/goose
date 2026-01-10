// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include "benchmark_configuration.hpp"
#include "benchmark.hpp"
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
