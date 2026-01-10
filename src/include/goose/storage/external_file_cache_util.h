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

#include <goose/common/enums/cache_validation_mode.h>
#include <goose/common/optional_ptr.h>

namespace goose {

struct OpenFileInfo;
class DatabaseInstance;
class ClientContext;

struct ExternalFileCacheUtil {
	//! Get the cache validation mode in order of file open info, client context (for client-local settings), or
	//! database config.
	static CacheValidationMode GetCacheValidationMode(const OpenFileInfo &info,
	                                                  optional_ptr<ClientContext> client_context, DatabaseInstance &db);
};

} // namespace goose
