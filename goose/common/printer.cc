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
//


#include <goose/common/printer.h>
#include <goose/common/progress_bar/progress_bar.h>
#include <goose/common/windows_util.h>
#include <goose/common/windows.h>
#include <stdio.h>

#ifndef GOOSE_DISABLE_PRINT
#ifdef GOOSE_WINDOWS
#include <io.h>
#else
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#endif
#endif

namespace goose {
    void Printer::RawPrint(OutputStream stream, const string &str) {
#ifndef GOOSE_DISABLE_PRINT
#ifdef GOOSE_WINDOWS
        if (IsTerminal(stream)) {
            // print utf8 to terminal
            auto unicode = WindowsUtil::UTF8ToMBCS(str.c_str());
            fprintf(stream == OutputStream::STREAM_STDERR ? stderr : stdout, "%s", unicode.c_str());
            return;
        }
#endif
        fprintf(stream == OutputStream::STREAM_STDERR ? stderr : stdout, "%s", str.c_str());
#endif
    }

    void Printer::DefaultLinePrint(OutputStream stream, const string &str) {
        Printer::RawPrint(stream, str + "\n");
    }

    line_printer_f Printer::line_printer = Printer::DefaultLinePrint;

    // LCOV_EXCL_START
    void Printer::Print(OutputStream stream, const string &str) {
        Printer::line_printer(stream, str);
    }

    void Printer::Flush(OutputStream stream) {
#ifndef GOOSE_DISABLE_PRINT
        fflush(stream == OutputStream::STREAM_STDERR ? stderr : stdout);
#endif
    }

    void Printer::Print(const string &str) {
        Printer::Print(OutputStream::STREAM_STDERR, str);
    }

    bool Printer::IsTerminal(OutputStream stream) {
#ifndef GOOSE_DISABLE_PRINT
#ifdef GOOSE_WINDOWS
        auto stream_handle = stream == OutputStream::STREAM_STDERR ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE;
        return GetFileType(GetStdHandle(stream_handle)) == FILE_TYPE_CHAR;
#else
        return isatty(stream == OutputStream::STREAM_STDERR ? 2 : 1);
#endif
#else
        throw InternalException("IsTerminal called while printing is disabled");
#endif
    }

    idx_t Printer::TerminalWidth() {
#ifndef GOOSE_DISABLE_PRINT
#ifdef GOOSE_WINDOWS
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int rows;

        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        rows = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        return rows;
#else
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);
        return w.ws_col;
#endif
#else
        throw InternalException("TerminalWidth called while printing is disabled");
#endif
    }

    // LCOV_EXCL_STOP
} // namespace goose
