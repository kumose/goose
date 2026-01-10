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
