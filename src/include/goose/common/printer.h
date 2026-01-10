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
