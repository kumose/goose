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

enum class FileGlobOptions : uint8_t { DISALLOW_EMPTY = 0, ALLOW_EMPTY = 1, FALLBACK_GLOB = 2 };

struct FileGlobInput {
	FileGlobInput(FileGlobOptions options) // NOLINT: allow implicit conversion from FileGlobOptions
	    : behavior(options) {
	}
	FileGlobInput(FileGlobOptions options, string extension_p) : behavior(options), extension(std::move(extension_p)) {
	}

	FileGlobOptions behavior;
	string extension;
};

} // namespace goose
