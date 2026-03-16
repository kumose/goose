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

#include <goose/common/types.h>
#include <goose/optimizer/matcher/type_matcher.h>
namespace goose {

//! The TypeMatcherId class contains a set of matchers that can be used to pattern match TypeIds for Rules
class TypeMatcherId : public TypeMatcher {
public:
	explicit TypeMatcherId(LogicalTypeId type_id_p) : type_id(type_id_p) {
	}

	bool Match(const LogicalType &type) override {
		return type.id() == this->type_id;
	}

private:
	LogicalTypeId type_id;
};

} // namespace goose
