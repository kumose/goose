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

#include <goose/common/string.h>
#include <goose/common/vector.h>
#include <goose/main/database.h>

#if defined(GENERATED_EXTENSION_HEADERS) && !defined(GOOSE_AMALGAMATION)
#include <goose/common/common.h>
#include "generated_extension_headers.h"

namespace goose {

vector<string> LinkedExtensions();
vector<string> LoadedExtensionTestPaths();

} // namespace goose
#endif
