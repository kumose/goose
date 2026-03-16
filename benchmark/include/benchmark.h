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

#include <memory>
#include "benchmark_configuration.hpp"
#include <goose/common/vector.h>
#include <goose/common/string.h>
#include <goose/common/helper.h>
#include <goose/common/unique_ptr.h>

namespace goose {

//! Base class for any state that has to be kept by a Benchmark
struct BenchmarkState {
	virtual ~BenchmarkState() {
	}
};

//! The base Benchmark class is a base class that is used to create and register
//! new benchmarks
class Benchmark {
	constexpr static size_t DEFAULT_NRUNS = 5;
	Benchmark(Benchmark &) = delete;

public:
	//! The name of the benchmark
	string name;
	//! The benchmark group this benchmark belongs to
	string group;

	Benchmark(bool register_benchmark, string name, string group);

	//! Initialize the benchmark state
	virtual goose::unique_ptr<BenchmarkState> Initialize(BenchmarkConfiguration &config) {
		return nullptr;
	}
	//! Assert correctness after load, before run
	virtual void Assert(BenchmarkState *state) {};
	//! Run the benchmark
	virtual void Run(BenchmarkState *state) = 0;
	//! Cleanup the benchmark, called after each Run
	virtual void Cleanup(BenchmarkState *state) = 0;
	//! Verify that the output of the benchmark was correct
	virtual string Verify(BenchmarkState *state) = 0;
	//! Finalize the benchmark runner
	virtual void Finalize() {
	}
	virtual string GetQuery() {
		return string();
	}
	virtual string DisplayName() {
		return name;
	}
	virtual string Group() {
		return group;
	}
	virtual string Subgroup() {
		return string();
	}
	//! Interrupt the benchmark because of a timeout
	virtual void Interrupt(BenchmarkState *state) = 0;
	//! Returns information about the benchmark
	virtual string BenchmarkInfo() = 0;

	string GetInfo() {
		return name + " - " + group + "\n" + BenchmarkInfo();
	}

	virtual string GetLogOutput(BenchmarkState *state) = 0;

	//! Whether or not Initialize() should be called once for every run or just
	//! once
	virtual bool RequireReinit() {
		return false;
	}
	//! The amount of runs to do for this benchmark
	virtual size_t NRuns() {
		return DEFAULT_NRUNS;
	}
	//! The timeout for this benchmark (in seconds)
	virtual optional_idx Timeout(const BenchmarkConfiguration &config) {
		return config.timeout_duration;
	}
};

} // namespace goose
