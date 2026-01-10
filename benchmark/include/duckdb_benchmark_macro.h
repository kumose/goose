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
