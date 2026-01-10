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
