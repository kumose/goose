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

namespace goose {
    struct searchMatch;

    enum class tokenType : uint8_t {
        TOKEN_IDENTIFIER,
        TOKEN_NUMERIC_CONSTANT,
        TOKEN_STRING_CONSTANT,
        TOKEN_OPERATOR,
        TOKEN_KEYWORD,
        TOKEN_COMMENT,
        TOKEN_CONTINUATION,
        TOKEN_CONTINUATION_SELECTED,
        TOKEN_BRACKET,
        TOKEN_ERROR
    };

    enum class HighlightingType { KEYWORD, CONSTANT, COMMENT, ERROR_TOKEN, CONTINUATION, CONTINUATION_SELECTED };

    enum class ExtraHighlightingType { NONE, UNDERLINE, BOLD };

    struct highlightToken {
        tokenType type;
        size_t start = 0;
        ExtraHighlightingType extra_highlighting = ExtraHighlightingType::NONE;
    };

    struct ExtraHighlighting {
        ExtraHighlighting() : start(0), end(0), type(ExtraHighlightingType::NONE) {
        }

        idx_t start;
        idx_t end;
        ExtraHighlightingType type;
    };

    class Highlighting {
    public:
        static void Enable();

        static void Disable();

        static bool IsEnabled();

        static void SetHighlightingColor(HighlightingType type, const char *color);

        static vector<highlightToken> Tokenize(char *buf, size_t len, bool is_dot_command);

        static void AddExtraHighlighting(size_t len, vector<highlightToken> &tokens,
                                         ExtraHighlighting extra_highlighting);

        static string HighlightText(char *buf, size_t len, size_t start_pos, size_t end_pos,
                                    const vector<highlightToken> &tokens);
    };
} // namespace goose
