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

#include <goose/function/scalar_function.h>
#include <goose/function/function_set.h>
#include <goose/function/built_in_functions.h>

namespace goose {

struct StructExtractBindData : public FunctionData {
	explicit StructExtractBindData(idx_t index) : index(index) {
	}

	idx_t index;

public:
	unique_ptr<FunctionData> Copy() const override {
		return make_uniq<StructExtractBindData>(index);
	}
	bool Equals(const FunctionData &other_p) const override {
		auto &other = other_p.Cast<StructExtractBindData>();
		return index == other.index;
	}
};

} // namespace goose
