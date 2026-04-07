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

#include <goose/common/progress_bar/display/terminal_progress_bar_display.h>
#include <goose/shell/shell_prompt.h>

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
        vector<unique_ptr<ProgressBarPrompt> > components;
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
