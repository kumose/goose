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
