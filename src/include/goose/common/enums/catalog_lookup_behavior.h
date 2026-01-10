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

#include <goose/common/constants.h>

namespace goose {

//! Enum used for indicating lookup behavior of specific catalog types
// STANDARD means the catalog lookups are performed in a regular manner (i.e. according to the users' search path)
// LOWER_PRIORITY means the catalog lookups are de-prioritized and we do lookups in other catalogs first
// NEVER_LOOKUP means we never do lookups for this specific type in this catalog
enum class CatalogLookupBehavior : uint8_t { STANDARD = 0, LOWER_PRIORITY = 1, NEVER_LOOKUP = 2 };

} // namespace goose
