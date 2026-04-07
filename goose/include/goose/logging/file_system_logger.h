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
