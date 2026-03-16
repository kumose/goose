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

#ifdef _MSC_VER
// these break enum.hpp otherwise
#undef DELETE
#undef DEFAULT
#undef EXISTS
#undef IN
// this breaks file_system.cc otherwise
#undef CreateDirectory
#undef RemoveDirectory
#endif

#include "compare_result.h"
#include <goose/goose.h>
#include <goose/common/string_util.h>
#include <goose/common/enum_util.h>
#include <goose/common/types.h>
#include "test_config.h"
#include <sstream>
#include <iostream>

namespace goose {

void RegisterSqllogictests();
bool SummarizeFailures();

void DeleteDatabase(string path);
void TestDeleteDirectory(string path);
void TestCreateDirectory(string path);
string TestJoinPath(string path1, string path2);
void TestDeleteFile(string path);
void TestChangeDirectory(string path);

void SetDeleteTestPath(bool delete_path);
bool DeleteTestPath();
void ClearTestDirectory();
string TestGetCurrentDirectory();
string TestDirectoryPath();
string TestCreatePath(string suffix);
unique_ptr<DBConfig> GetTestConfig();
bool TestIsInternalError(unordered_set<string> &internal_error_messages, const string &error);
void SetTestDirectory(string path);
void SetDebugInitialize(int value);
void AddRequire(string require);
bool IsRequired(string require);

bool NO_FAIL(QueryResult &result);
bool NO_FAIL(goose::unique_ptr<QueryResult> result);

#define REQUIRE_NO_FAIL(result) REQUIRE(NO_FAIL((result)))
#define REQUIRE_FAIL(result)    REQUIRE((result)->HasError())

#define COMPARE_CSV(result, csv, header)                                                                               \
	{                                                                                                                  \
		auto res = compare_csv(*result, csv, header);                                                                  \
		if (!res.empty())                                                                                              \
			FAIL(res);                                                                                                 \
	}

#define COMPARE_CSV_COLLECTION(collection, csv, header)                                                                \
	{                                                                                                                  \
		auto res = compare_csv_collection(collection, csv, header);                                                    \
		if (!res.empty())                                                                                              \
			FAIL(res);                                                                                                 \
	}

} // namespace goose
