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
#include <goose/common/enums/on_entry_not_found.h>

namespace goose {

struct DetachInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::DETACH_INFO;

public:
	DetachInfo();

	//! The alias of the attached database
	string name;
	//! Whether to throw an exception if alias is not found
	OnEntryNotFound if_not_found;

public:
	unique_ptr<DetachInfo> Copy() const;
	string ToString() const;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);
};
} // namespace goose
