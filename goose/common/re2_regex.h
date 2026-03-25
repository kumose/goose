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


// RE2 compatibility layer with std::regex

#pragma once

#include <goose/common/winapi.h>
#include <goose/common/vector.h>
#include <goose/common/shared_ptr.h>
#include <goose/common/string.h>
#include <stdexcept>
#include <xre2/re2.h>

namespace goose {
    enum class RegexOptions : uint8_t { NONE, CASE_INSENSITIVE };

    class Regex {
    public:
        GOOSE_API explicit Regex(const std::string &pattern, RegexOptions options = RegexOptions::NONE);

        explicit Regex(const char *pattern, RegexOptions options = RegexOptions::NONE) : Regex(std::string(pattern)) {
        }

        const RE2 &GetRegex() const {
            return *regex;
        }

    private:
        goose::shared_ptr<RE2> regex;
    };

    struct GroupMatch {
        std::string text;
        uint32_t position;

        const std::string &str() const { // NOLINT
            return text;
        }

        operator std::string() const { // NOLINT: allow implicit cast
            return text;
        }
    };

    struct Match {
        goose::vector<GroupMatch> groups;

        GroupMatch &GetGroup(uint64_t index) {
            if (index >= groups.size()) {
                throw std::runtime_error("RE2: Match index is out of range");
            }
            return groups[index];
        }

        std::string str(uint64_t index) { // NOLINT
            return GetGroup(index).text;
        }

        uint64_t position(uint64_t index) { // NOLINT
            return GetGroup(index).position;
        }

        uint64_t length(uint64_t index) { // NOLINT
            return GetGroup(index).text.size();
        }

        GroupMatch &operator[](uint64_t i) {
            return GetGroup(i);
        }
    };

    GOOSE_API bool RegexSearch(const std::string &input, Match &match, const Regex &regex);

    GOOSE_API bool RegexMatch(const std::string &input, Match &match, const Regex &regex);

    GOOSE_API bool RegexMatch(const char *start, const char *end, Match &match, const Regex &regex);

    GOOSE_API bool RegexMatch(const std::string &input, const Regex &regex);

    GOOSE_API goose::vector<Match> RegexFindAll(const std::string &input, const Regex &regex);

    GOOSE_API goose::vector<Match> RegexFindAll(const char *input_data, size_t input_size, const RE2 &regex);
} // namespace goose
