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

#include <array>
#include <atomic>
#include <bitset>
#include <chrono>
#include <fstream>
#include <iosfwd>
#include <list>
#include <map>
#ifdef __MVS__
#include <time.h>
#endif
#include <mutex>
#include <utility>
#include <set>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <memory>

#include <cctype>
#include <locale>
#include <sstream>

#include <turbo/base/macros.h>

namespace goose {
    using std::array;
    //! NOTE: When repeatedly trying to atomically set a value in a loop, you can use as the loop condition:
    //! * std::atomic_compare_exchange_weak
    //! * std::atomic::compare_exchange_weak
    //! If not used as a loop condition, use:
    //! * std::atomic_compare_exchange_strong
    //! * std::atomic::compare_exchange_strong
    //! If this is not done correctly, we may get correctness issues when using older compiler versions (see: issue #14389)
    //! Performance may be optimized using std::memory_order, but NOT at the cost of correctness.
    //! For correct examples of this, see concurrentqueue.h
    using std::atomic;
    using std::bitset;

    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::microseconds;
    using std::chrono::milliseconds;
    using std::chrono::nanoseconds;
    using std::chrono::steady_clock;
    using std::chrono::system_clock;
    using std::chrono::time_point;


    using std::endl;
    using std::fstream;
    using std::ifstream;
    using std::ios;
    using std::ios_base;
    using std::ofstream;

    using std::list;

    using std::map;
    using std::multimap;

    using std::lock_guard;
    using std::mutex;
    using std::unique_lock;

    using std::make_pair;
    using std::pair;

    using std::multiset;
    using std::set;

    using std::stack;
    using std::swap;
    using std::to_string;
    using std::unordered_set;
    using std::unordered_map;
}  // namespace goose

#define GOOSE_BASE_STD

namespace goose_base_std {
    using ::std::isspace;
    using ::std::make_shared;
    using ::std::shared_ptr;
    using ::std::unique_ptr;
    // using ::std::make_unique;
    using ::std::basic_stringstream;
    using ::std::stringstream;
    using ::std::wstringstream;

    using ::std::isspace;
} // namespace goose_base_std
