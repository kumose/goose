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

#include <goose/common/common.h>
#include <goose/common/string_util.h>

namespace goose {

enum class OutputStream : uint8_t { STREAM_STDOUT = 1, STREAM_STDERR = 2 };

typedef void (*line_printer_f)(OutputStream stream, const string &str);

//! Printer is a static class that allows printing to logs or stdout/stderr
class Printer {
public:
	//! Print the object to the stream
	GOOSE_API static void Print(OutputStream stream, const string &str);
	//! Print the object to stderr
	GOOSE_API static void Print(const string &str);
	//! Print the formatted object to the stream
	template <typename... ARGS>
	static void PrintF(OutputStream stream, const string &str, ARGS... params) {
		Printer::Print(stream, StringUtil::Format(str, params...));
	}
	//! Print the formatted object to stderr
	template <typename... ARGS>
	static void PrintF(const string &str, ARGS... params) {
		Printer::PrintF(OutputStream::STREAM_STDERR, str, std::forward<ARGS>(params)...);
	}
	//! Directly prints the string to stdout without a newline
	GOOSE_API static void RawPrint(OutputStream stream, const string &str);
	//! Flush an output stream
	GOOSE_API static void Flush(OutputStream stream);
	//! Whether or not we are printing to a terminal
	GOOSE_API static bool IsTerminal(OutputStream stream);
	//! The terminal width
	GOOSE_API static idx_t TerminalWidth();

	// hook to allow capturing the output and routing it somewhere else / reformat it};
	static line_printer_f line_printer;

private:
	static void DefaultLinePrint(OutputStream stream, const string &str);
};
} // namespace goose
