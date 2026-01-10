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
