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

#include <goose/planner/table_filter.h>
#include <goose/common/types/selection_vector.h>
#include <goose/execution/expression_executor.h>

namespace goose {

//! Thread-local state for executing a table filter
struct TableFilterState {
public:
	virtual ~TableFilterState() = default;

public:
	static unique_ptr<TableFilterState> Initialize(ClientContext &context, const TableFilter &filter);

public:
	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}
};

struct ConjunctionAndFilterState : public TableFilterState {
public:
	vector<unique_ptr<TableFilterState>> child_states;
};

struct ConjunctionOrFilterState : public TableFilterState {
public:
	vector<unique_ptr<TableFilterState>> child_states;
};

struct ExpressionFilterState : public TableFilterState {
public:
	ExpressionFilterState(ClientContext &context, const Expression &expression);

	ExpressionExecutor executor;
};

struct BFTableFilterState final : public TableFilterState {
	idx_t current_capacity;
	Vector hashes_v;
	Vector found_v;
	Vector keys_sliced_v;
	SelectionVector bf_sel;

	explicit BFTableFilterState(const LogicalType &key_logical_type)
	    : current_capacity(STANDARD_VECTOR_SIZE), hashes_v(LogicalType::HASH), found_v(LogicalType::UBIGINT),
	      keys_sliced_v(key_logical_type), bf_sel(STANDARD_VECTOR_SIZE) {
	}
};

} // namespace goose
