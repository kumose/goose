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
