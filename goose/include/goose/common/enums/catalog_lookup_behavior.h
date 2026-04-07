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

#include <goose/common/constants.h>

namespace goose {

//! Enum used for indicating lookup behavior of specific catalog types
// STANDARD means the catalog lookups are performed in a regular manner (i.e. according to the users' search path)
// LOWER_PRIORITY means the catalog lookups are de-prioritized and we do lookups in other catalogs first
// NEVER_LOOKUP means we never do lookups for this specific type in this catalog
enum class CatalogLookupBehavior : uint8_t { STANDARD = 0, LOWER_PRIORITY = 1, NEVER_LOOKUP = 2 };

} // namespace goose
