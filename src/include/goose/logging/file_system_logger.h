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

namespace goose {

#define GOOSE_LOG_FILE_SYSTEM_BYTES(HANDLE, OP, BYTES, POS)                                                           \
	{                                                                                                                  \
		if (HANDLE.logger) {                                                                                           \
			GOOSE_LOG(HANDLE.logger, FileSystemLogType, HANDLE, OP, BYTES, POS)                                       \
		}                                                                                                              \
	}
#define GOOSE_LOG_FILE_SYSTEM(HANDLE, OP)                                                                             \
	{                                                                                                                  \
		if (HANDLE.logger) {                                                                                           \
			GOOSE_LOG(HANDLE.logger, FileSystemLogType, HANDLE, OP)                                                   \
		}                                                                                                              \
	}

// Macros for logging to file handles
#define GOOSE_LOG_FILE_SYSTEM_READ(HANDLE, BYTES, POS)  GOOSE_LOG_FILE_SYSTEM_BYTES(HANDLE, "READ", BYTES, POS);
#define GOOSE_LOG_FILE_SYSTEM_WRITE(HANDLE, BYTES, POS) GOOSE_LOG_FILE_SYSTEM_BYTES(HANDLE, "WRITE", BYTES, POS);
#define GOOSE_LOG_FILE_SYSTEM_OPEN(HANDLE)              GOOSE_LOG_FILE_SYSTEM(HANDLE, "OPEN");
#define GOOSE_LOG_FILE_SYSTEM_CLOSE(HANDLE)             GOOSE_LOG_FILE_SYSTEM(HANDLE, "CLOSE");

} // namespace goose
