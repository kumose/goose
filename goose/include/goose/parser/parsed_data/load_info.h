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

#include <goose/parser/parsed_data/parse_info.h>

namespace goose {

enum class LoadType : uint8_t { LOAD, INSTALL, FORCE_INSTALL };

struct LoadInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::LOAD_INFO;

public:
	LoadInfo() : ParseInfo(TYPE) {
	}

	string filename;
	string repository;
	bool repo_is_alias;
	string version;
	LoadType load_type;

public:
	unique_ptr<LoadInfo> Copy() const;
	string ToString() const;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);
};

} // namespace goose
