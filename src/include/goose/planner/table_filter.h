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

#include <goose/common/common.h>
#include <goose/common/enums/filter_propagate_result.h>
#include <goose/common/types-import.h>
#include <goose/common/reference_map.h>
#include <goose/common/types.h>
#include <goose/common/types-import.h>
#include <goose/planner/column_binding.h>
#include <goose/common/column_index.h>

namespace goose {
class BaseStatistics;
class Expression;
class PhysicalOperator;
class PhysicalTableScan;

enum class TableFilterType : uint8_t {
	CONSTANT_COMPARISON = 0, // constant comparison (e.g. =C, >C, >=C, <C, <=C)
	IS_NULL = 1,             // C IS NULL
	IS_NOT_NULL = 2,         // C IS NOT NULL
	CONJUNCTION_OR = 3,      // OR of different filters
	CONJUNCTION_AND = 4,     // AND of different filters
	STRUCT_EXTRACT = 5,      // filter applies to child-column of struct
	OPTIONAL_FILTER = 6,     // executing filter is not required for query correctness
	IN_FILTER = 7,           // col IN (C1, C2, C3, ...)
	DYNAMIC_FILTER = 8,      // dynamic filters can be updated at run-time
	EXPRESSION_FILTER = 9,   // an arbitrary expression
	BLOOM_FILTER = 10,       // a probabilistic filter that can test whether a value is in a set of other value
};

//! TableFilter represents a filter pushed down into the table scan.
class TableFilter {
public:
	explicit TableFilter(TableFilterType filter_type_p) : filter_type(filter_type_p) {
	}
	virtual ~TableFilter() {
	}

	TableFilterType filter_type;

public:
	//! Returns true if the statistics indicate that the segment can contain values that satisfy that filter
	virtual FilterPropagateResult CheckStatistics(BaseStatistics &stats) const = 0;
	virtual string ToString(const string &column_name) const = 0;
	string DebugToString() const;
	virtual unique_ptr<TableFilter> Copy() const = 0;
	virtual bool Equals(const TableFilter &other) const {
		return filter_type == other.filter_type;
	}
	virtual unique_ptr<Expression> ToExpression(const Expression &column) const = 0;

	virtual void Serialize(Serializer &serializer) const;
	static unique_ptr<TableFilter> Deserialize(Deserializer &deserializer);

public:
	template <class TARGET>
	TARGET &Cast() {
		if (filter_type != TARGET::TYPE) {
			throw InternalException("Failed to cast to type - table filter type mismatch");
		}
		return reinterpret_cast<TARGET &>(*this);
	}

	template <class TARGET>
	const TARGET &Cast() const {
		if (filter_type != TARGET::TYPE) {
			throw InternalException("Failed to cast to type - table filter type mismatch");
		}
		return reinterpret_cast<const TARGET &>(*this);
	}
};

//! The filters in here are non-composite (only need a single column to be evaluated)
//! Conditions like `A = 2 OR B = 4` are not pushed into a TableFilterSet.
class TableFilterSet {
public:
	map<idx_t, unique_ptr<TableFilter>> filters;

public:
	void PushFilter(const ColumnIndex &col_idx, unique_ptr<TableFilter> filter);

	bool Equals(TableFilterSet &other) {
		if (filters.size() != other.filters.size()) {
			return false;
		}
		for (auto &entry : filters) {
			auto other_entry = other.filters.find(entry.first);
			if (other_entry == other.filters.end()) {
				return false;
			}
			if (!entry.second->Equals(*other_entry->second)) {
				return false;
			}
		}
		return true;
	}
	static bool Equals(TableFilterSet *left, TableFilterSet *right) {
		if (left == right) {
			return true;
		}
		if (!left || !right) {
			return false;
		}
		return left->Equals(*right);
	}

	unique_ptr<TableFilterSet> Copy() const {
		auto copy = make_uniq<TableFilterSet>();
		for (auto &it : filters) {
			copy->filters.emplace(it.first, it.second->Copy());
		}
		return copy;
	}

	void Serialize(Serializer &serializer) const;
	static TableFilterSet Deserialize(Deserializer &deserializer);
};

class DynamicTableFilterSet {
public:
	void ClearFilters(const PhysicalOperator &op);
	void PushFilter(const PhysicalOperator &op, idx_t column_index, unique_ptr<TableFilter> filter);

	bool HasFilters() const;
	unique_ptr<TableFilterSet> GetFinalTableFilters(const PhysicalTableScan &scan,
	                                                optional_ptr<TableFilterSet> existing_filters) const;

private:
	mutable mutex lock;
	reference_map_t<const PhysicalOperator, unique_ptr<TableFilterSet>> filters;
};

} // namespace goose
