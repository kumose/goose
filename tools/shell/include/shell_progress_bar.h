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

#include <goose/common/progress_bar/display/terminal_progress_bar_display.h>
#include "shell_prompt.h"

namespace goose_shell {
struct ProgressBarPrompt;

struct ShellProgressBar {
	friend class ShellProgressBarDisplay;
	friend struct ProgressBarPrompt;

public:
	ShellProgressBar();
	~ShellProgressBar();

public:
	void AddComponent(const string &text);
	void ClearComponents();
	string GenerateProgressBar(ShellState &state, idx_t terminal_width);

private:
	vector<unique_ptr<ProgressBarPrompt>> components;
	goose::ProgressBarDisplayInfo display_info;
	int32_t percentage = 0;
	double estimated_remaining_seconds = 0;
	unique_ptr<goose::Connection> connection;
};

//! Displays a status bar alongside the progress bar
class ShellProgressBarDisplay : public goose::TerminalProgressBarDisplay {
public:
	ShellProgressBarDisplay();

public:
	void Finish() override;

protected:
	void PrintProgressInternal(int32_t percentage, double estimated_remaining_seconds, bool is_finished) override;

private:
	optional_idx previous_terminal_width;
};

} // namespace goose_shell
