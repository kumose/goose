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
