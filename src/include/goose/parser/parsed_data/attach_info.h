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
#include <goose/common/types-import.h>
#include <goose/common/types/value.h>
#include <goose/common/enums/on_create_conflict.h>
#include <goose/parser/parsed_expression.h>

namespace goose {

struct AttachInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::ATTACH_INFO;

public:
	AttachInfo() : ParseInfo(TYPE) {
	}

	//! The alias of the attached database
	string name;
	//! The path to the attached database
	string path;
	//! Set of (key, value) options
	case_insensitive_map_t<unique_ptr<ParsedExpression>> parsed_options;
	//! Set of bound (key, value) options
	unordered_map<string, Value> options;
	//! What to do on create conflict
	OnCreateConflict on_conflict = OnCreateConflict::ERROR_ON_CONFLICT;

public:
	//! Copies this AttachInfo and returns an unique pointer to the new AttachInfo.
	unique_ptr<AttachInfo> Copy() const;
	string ToString() const;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);
};

} // namespace goose
