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

#include "goose_benchmark.hpp"

#define GOOSE_BENCHMARK(NAME, GROUP)                                                                                  \
	class NAME##Benchmark : public GooseBenchmark {                                                                   \
		NAME##Benchmark(bool register_benchmark) : GooseBenchmark(register_benchmark, "" #NAME, GROUP) {              \
		}                                                                                                              \
                                                                                                                       \
	public:                                                                                                            \
		static NAME##Benchmark *GetInstance() {                                                                        \
			static NAME##Benchmark singleton(true);                                                                    \
			auto benchmark = goose::unique_ptr<GooseBenchmark>(new NAME##Benchmark(false));                          \
			return &singleton;                                                                                         \
		}

#define REGISTER_BENCHMARK(NAME) auto global_instance_##NAME = NAME##Benchmark::GetInstance()

#define FINISH_BENCHMARK(NAME)                                                                                         \
	}                                                                                                                  \
	;                                                                                                                  \
	REGISTER_BENCHMARK(NAME);

namespace goose {}
