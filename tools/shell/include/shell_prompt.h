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

#include "shell_state.h"
#include "shell_highlight.h"

namespace goose_shell {

enum class PromptComponentType { LITERAL, SQL, SET_COLOR, SET_INTENSITY, SET_HIGHLIGHT_ELEMENT, RESET_COLOR, SETTING };

struct PromptComponent {
	PromptComponentType type;
	string literal;
	PrintColor color;
	PrintIntensity intensity;
	HighlightElementType highlight_element;
};

// Supports dynamic prompts
// Example prompt:
// {max_length:40}{color:green}{color:bold}{setting:current_database_and_schema}{color:reset} D "
struct Prompt {
public:
	virtual ~Prompt() = default;

	void ParsePrompt(const string &prompt);
	string GeneratePrompt(ShellState &state);
	void PrintPrompt(ShellState &state, PrintOutput output);

protected:
	vector<PromptComponent> components;
	optional_idx max_length;

protected:
	void AddLiteral(const string &str);
	void AddComponent(const string &bracket_type, const string &value);
	string EvaluateSQL(ShellState &state, const string &sql);
	string HandleColor(const PromptComponent &component);
	virtual bool ParseSetting(const string &bracket_type, const string &value);
	virtual string HandleSetting(ShellState &state, const PromptComponent &component);
	virtual vector<string> GetSupportedSettings();
	string HandleText(ShellState &state, const string &text, idx_t &length);
	string ExecuteSQL(ShellState &state, const string &query);
	virtual goose::Connection &GetConnection(ShellState &state);
};

} // namespace goose_shell
