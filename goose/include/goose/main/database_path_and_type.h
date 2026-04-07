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

#include <string>
#include <goose/main/config.h>

namespace goose {

class QueryContext;

struct DBPathAndType {
	//! Parse database extension type and rest of path from combined form (type:path)
	static void ExtractExtensionPrefix(string &path, string &db_type);
	//! Check the magic bytes of a file and set the database type based on that
	static void CheckMagicBytes(QueryContext context, FileSystem &fs, string &path, string &db_type);

	//! Run ExtractExtensionPrefix followed by CheckMagicBytes
	static void ResolveDatabaseType(FileSystem &fs, string &path, string &db_type);
};
} // namespace goose
