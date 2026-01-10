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

#include <goose/function/function_set.h>
#include <goose/function/scalar_function.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/function/built_in_functions.h>
#include <goose/common/serializer/serializer.h>
#include <goose/common/serializer/deserializer.h>

namespace goose {

struct ListArgFunctor {
	static Vector &GetList(Vector &list) {
		return list;
	}
	static idx_t GetListSize(Vector &list) {
		return ListVector::GetListSize(list);
	}
	static Vector &GetEntry(Vector &list) {
		return ListVector::GetEntry(list);
	}
};

struct ContainsFunctor {
	static inline bool Initialize() {
		return false;
	}
	static inline bool UpdateResultEntries(idx_t child_idx) {
		return true;
	}
};

struct PositionFunctor {
	static inline int32_t Initialize() {
		return 0;
	}
	static inline int32_t UpdateResultEntries(idx_t child_idx) {
		return UnsafeNumericCast<int32_t>(child_idx + 1);
	}
};

struct MapUtil {
	static void ReinterpretMap(Vector &target, Vector &other, idx_t count);
};

struct VariableReturnBindData : public FunctionData {
	LogicalType stype;

	explicit VariableReturnBindData(LogicalType stype_p) : stype(std::move(stype_p)) {
	}

	unique_ptr<FunctionData> Copy() const override {
		return make_uniq<VariableReturnBindData>(stype);
	}
	bool Equals(const FunctionData &other_p) const override {
		auto &other = other_p.Cast<VariableReturnBindData>();
		return stype == other.stype;
	}
	static void Serialize(Serializer &serializer, const optional_ptr<FunctionData> bind_data,
	                      const ScalarFunction &function) {
		auto &info = bind_data->Cast<VariableReturnBindData>();
		serializer.WriteProperty(100, "variable_return_type", info.stype);
	}

	static unique_ptr<FunctionData> Deserialize(Deserializer &deserializer, ScalarFunction &bound_function) {
		auto stype = deserializer.ReadProperty<LogicalType>(100, "variable_return_type");
		return make_uniq<VariableReturnBindData>(std::move(stype));
	}
};

template <class T, class MAP_TYPE = map<T, idx_t>>
struct HistogramAggState {
	MAP_TYPE *hist;
};

ScalarFunction GetKeyExtractFunction();
ScalarFunction GetIndexExtractFunction();
ScalarFunction GetExtractAtFunction();

} // namespace goose
