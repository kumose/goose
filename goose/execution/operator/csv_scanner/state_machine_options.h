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

#include <goose/execution/operator/csv_scanner/csv_option.h>

namespace goose {
//! Struct that holds the configuration of a CSV State Machine
//! Basically which char, quote and escape were used to generate it.
struct CSVStateMachineOptions {
	CSVStateMachineOptions() {};
	CSVStateMachineOptions(string delimiter_p, char quote_p, char escape_p, char comment_p,
	                       NewLineIdentifier new_line_p, bool strict_mode_p)
	    : delimiter(std::move(delimiter_p)), quote(quote_p), escape(escape_p), comment(comment_p), new_line(new_line_p),
	      strict_mode(strict_mode_p) {};

	//! Delimiter to separate columns within each line
	CSVOption<string> delimiter {","};
	//! Quote used for columns that contain reserved characters, e.g '
	CSVOption<char> quote = '\"';
	//! Escape character to escape quote character
	CSVOption<char> escape = '\0';
	//! Comment character to skip a line
	CSVOption<char> comment = '\0';
	//! New Line separator
	CSVOption<NewLineIdentifier> new_line = NewLineIdentifier::NOT_SET;
	//! How Strict the parser should be
	CSVOption<bool> strict_mode = true;

	bool operator==(const CSVStateMachineOptions &other) const {
		return delimiter == other.delimiter && quote == other.quote && escape == other.escape &&
		       new_line == other.new_line && comment == other.comment && strict_mode == other.strict_mode;
	}
};
} // namespace goose
