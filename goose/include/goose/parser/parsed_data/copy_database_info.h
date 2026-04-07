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

#include <goose/common/unique_ptr.h>
#include <goose/common/vector.h>
#include <goose/parser/parsed_data/create_info.h>
#include <goose/parser/parsed_data/parse_info.h>

namespace goose {

struct CopyDatabaseInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::COPY_DATABASE_INFO;

public:
	explicit CopyDatabaseInfo() : ParseInfo(TYPE), target_database(INVALID_CATALOG) {
	}

	explicit CopyDatabaseInfo(const string &target_database) : ParseInfo(TYPE), target_database(target_database) {
	}

	// The destination database to which catalog entries are being copied
	string target_database;

	// The catalog entries that are going to be created in the destination DB
	vector<unique_ptr<CreateInfo>> entries;

public:
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);
};

} // namespace goose
