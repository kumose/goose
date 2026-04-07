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

struct UpdateExtensionsInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::UPDATE_EXTENSIONS_INFO;

public:
	UpdateExtensionsInfo() : ParseInfo(TYPE) {
	}

	vector<string> extensions_to_update;

public:
	unique_ptr<UpdateExtensionsInfo> Copy() const {
		auto result = make_uniq<UpdateExtensionsInfo>();
		result->extensions_to_update = extensions_to_update;
		return result;
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);
};

} // namespace goose
