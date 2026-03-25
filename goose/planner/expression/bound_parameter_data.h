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

#include <goose/common/types/value.h>
#include <goose/common/case_insensitive_map.h>

namespace goose {

struct BoundParameterData {
public:
	BoundParameterData() {
	}
	explicit BoundParameterData(Value val) : value(std::move(val)), return_type(GetDefaultType(value.type())) {
	}
	BoundParameterData(Value val, LogicalType type_p) : value(std::move(val)), return_type(std::move(type_p)) {
	}

private:
	Value value;

public:
	LogicalType return_type;

public:
	void SetValue(Value val) {
		value = std::move(val);
	}

	const Value &GetValue() const {
		return value;
	}

	void Serialize(Serializer &serializer) const;
	static shared_ptr<BoundParameterData> Deserialize(Deserializer &deserializer);

private:
	LogicalType GetDefaultType(const LogicalType &type) {
		if (value.type().id() == LogicalTypeId::VARCHAR && StringType::GetCollation(type).empty()) {
			return LogicalTypeId::STRING_LITERAL;
		}
		return value.type();
	}
};

} // namespace goose
