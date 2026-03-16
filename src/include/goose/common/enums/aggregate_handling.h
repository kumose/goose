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

//===----
enum class AggregateHandling : uint8_t {
	STANDARD_HANDLING,     // standard handling as in the SELECT clause
	NO_AGGREGATES_ALLOWED, // no aggregates allowed: any aggregates in this node will result in an error
	FORCE_AGGREGATES       // force aggregates: any non-aggregate select list entry will become a GROUP
};

const char *ToString(AggregateHandling value);

} // namespace goose
