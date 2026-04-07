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

#include <goose/common/types-import.h>
#include <goose/common/helper.h>

namespace goose {
    //! Profiler class to measure the elapsed time.
    template<typename T>
    class BaseProfiler {
    public:
        //! Start the timer.
        void Start() {
            finished = false;
            ran = true;
            start = Tick();
        }

        //! End the timer.
        void End() {
            end = Tick();
            finished = true;
        }

        //! Reset the timer.
        void Reset() {
            finished = false;
            ran = false;
        }

        //! Returns the elapsed time in seconds.
        //! If ran is false, it returns 0.
        //! If End() has been called, it returns the total elapsed time,
        //! otherwise, returns how far along the timer is right now.
        double Elapsed() const {
            if (!ran) {
                return 0;
            }
            auto measured_end = finished ? end : Tick();
            return std::chrono::duration_cast<std::chrono::duration<double> >(measured_end - start).count();
        }

        idx_t ElapsedNanos() const {
            if (!ran) {
                return 0;
            }
            auto measured_end = finished ? end : Tick();
            return static_cast<idx_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(measured_end - start).
                count());
        }

    private:
        //! Current time point.
        time_point<T> Tick() const {
            return T::now();
        }

        //! Start time point.
        time_point<T> start;
        //! End time point.
        time_point<T> end;
        //! True, if end End() been called.
        bool finished = false;
        //! True, if the timer was ran.
        bool ran = false;
    };

    using Profiler = BaseProfiler<steady_clock>;
} // namespace goose
