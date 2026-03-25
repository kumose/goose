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

namespace goose {

//! The AT clause specifies which version of a table to read
class BoundAtClause {
public:
	BoundAtClause(string unit_p, Value value_p) : unit(std::move(unit_p)), val(std::move(value_p)) {
	}

public:
	const string &Unit() const {
		return unit;
	}
	const Value &GetValue() const {
		return val;
	}

private:
	//! The unit (e.g. TIMESTAMP or VERSION)
	string unit;
	//! The value that is associated with the unit (e.g. TIMESTAMP '2020-01-01')
	Value val;
};

} // namespace goose
