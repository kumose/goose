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

#include <goose/common/types-import.h>
#include <goose/common/helper.h>

namespace goose {

//! Profiler class to measure the elapsed time.
template <typename T>
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
		return std::chrono::duration_cast<std::chrono::duration<double>>(measured_end - start).count();
	}

	idx_t ElapsedNanos() const {
		if (!ran) {
			return 0;
		}
		auto measured_end = finished ? end : Tick();
		return static_cast<idx_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(measured_end - start).count());
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
