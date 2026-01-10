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

#include <goose/goose-c.h>
#include <goose/execution/executor.h>
#include <goose/common/types-import.h>
#include <goose/common/profiler.h>
#include <goose/common/progress_bar/progress_bar_display.h>

namespace goose {

struct ClientConfig;
typedef unique_ptr<ProgressBarDisplay> (*progress_bar_display_create_func_t)();

struct QueryProgress {
	friend class ProgressBar;

public:
	QueryProgress();
	void Initialize();
	void Restart();
	double GetPercentage();
	uint64_t GetRowsProcesseed();
	uint64_t GetTotalRowsToProcess();
	QueryProgress &operator=(const QueryProgress &other);
	QueryProgress(const QueryProgress &other);

private:
	atomic<double> percentage;
	atomic<uint64_t> rows_processed;
	atomic<uint64_t> total_rows_to_process;
};

class ProgressBar {
public:
	static unique_ptr<ProgressBarDisplay> DefaultProgressBarDisplay();
	static void SystemOverrideCheck(ClientConfig &config);

	explicit ProgressBar(
	    Executor &executor, idx_t show_progress_after,
	    progress_bar_display_create_func_t create_display_func = ProgressBar::DefaultProgressBarDisplay);

	//! Starts the thread
	void Start();
	//! Updates the progress bar and prints it to the screen
	void Update(bool final);
	QueryProgress GetDetailedQueryProgress();
	void PrintProgress(double percentage);
	void FinishProgressBarPrint();
	bool ShouldPrint(bool final) const;
	bool PrintEnabled() const;

private:
	//! The executor
	Executor &executor;
	//! The profiler used to measure the time since the progress bar was started
	Profiler profiler;
	//! The time in ms after which to start displaying the progress bar
	idx_t show_progress_after;
	//! Keeps track of the total progress of a query
	QueryProgress query_progress;
	//! The display used to print the progress
	unique_ptr<ProgressBarDisplay> display;
	//! Whether or not profiling is supported for the current query
	bool supported = true;
	//! Whether the bar has already finished
	bool finished = false;
};
} // namespace goose
