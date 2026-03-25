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
    class History {
    public:
        static void Free();

        static idx_t GetLength();

        static const char *GetEntry(idx_t index);

        static void Overwrite(idx_t index, const char *new_entry);

        static void RemoveLastEntry();

        static int Add(const char *line);

        static int Add(const char *line, idx_t len);

        static int SetMaxLength(idx_t len);

        static int Save(const char *filename);

        static int Load(const char *filename);
    };
} // namespace goose
