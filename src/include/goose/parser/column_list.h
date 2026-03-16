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

#include <goose/parser/column_definition.h>

namespace goose {

//! A set of column definitions
class ColumnList {
public:
	class ColumnListIterator;

public:
	GOOSE_API explicit ColumnList(bool allow_duplicate_names = false);
	GOOSE_API explicit ColumnList(vector<ColumnDefinition> columns, bool allow_duplicate_names = false);

	GOOSE_API void AddColumn(ColumnDefinition column);
	void Finalize();

	GOOSE_API const ColumnDefinition &GetColumn(LogicalIndex index) const;
	GOOSE_API const ColumnDefinition &GetColumn(PhysicalIndex index) const;
	GOOSE_API const ColumnDefinition &GetColumn(const string &name) const;
	GOOSE_API ColumnDefinition &GetColumnMutable(LogicalIndex index);
	GOOSE_API ColumnDefinition &GetColumnMutable(PhysicalIndex index);
	GOOSE_API ColumnDefinition &GetColumnMutable(const string &name);
	GOOSE_API vector<string> GetColumnNames() const;
	GOOSE_API vector<LogicalType> GetColumnTypes() const;

	GOOSE_API bool ColumnExists(const string &name) const;

	GOOSE_API LogicalIndex GetColumnIndex(string &column_name) const;
	GOOSE_API PhysicalIndex LogicalToPhysical(LogicalIndex index) const;
	GOOSE_API LogicalIndex PhysicalToLogical(PhysicalIndex index) const;

	idx_t LogicalColumnCount() const {
		return columns.size();
	}
	idx_t PhysicalColumnCount() const {
		return physical_columns.size();
	}
	bool empty() const { // NOLINT: match stl API
		return columns.empty();
	}

	ColumnList Copy() const;
	void Serialize(Serializer &serializer) const;
	static ColumnList Deserialize(Deserializer &deserializer);

	GOOSE_API ColumnListIterator Logical() const;
	GOOSE_API ColumnListIterator Physical() const;

	void SetAllowDuplicates(bool allow_duplicates) {
		allow_duplicate_names = allow_duplicates;
	}

private:
	vector<ColumnDefinition> columns;
	//! A map of column name to column index
	case_insensitive_map_t<column_t> name_map;
	//! The set of physical columns
	vector<idx_t> physical_columns;
	//! Allow duplicate names or not
	bool allow_duplicate_names;

private:
	void AddToNameMap(ColumnDefinition &column);

public:
	// logical iterator
	class ColumnListIterator {
	public:
		ColumnListIterator(const ColumnList &list, bool physical) : list(list), physical(physical) {
		}

	private:
		const ColumnList &list;
		bool physical;

	private:
		class ColumnLogicalIteratorInternal {
		public:
			ColumnLogicalIteratorInternal(const ColumnList &list, bool physical, idx_t pos, idx_t end)
			    : list(list), physical(physical), pos(pos), end(end) {
			}

			const ColumnList &list;
			bool physical;
			idx_t pos;
			idx_t end;

		public:
			ColumnLogicalIteratorInternal &operator++() {
				pos++;
				return *this;
			}
			bool operator!=(const ColumnLogicalIteratorInternal &other) const {
				return pos != other.pos || end != other.end || &list != &other.list;
			}
			const ColumnDefinition &operator*() const {
				if (physical) {
					return list.GetColumn(PhysicalIndex(pos));
				} else {
					return list.GetColumn(LogicalIndex(pos));
				}
			}
		};

	public:
		idx_t Size() {
			return physical ? list.PhysicalColumnCount() : list.LogicalColumnCount();
		}

		ColumnLogicalIteratorInternal begin() { // NOLINT: match stl API
			return ColumnLogicalIteratorInternal(list, physical, 0, Size());
		}
		ColumnLogicalIteratorInternal end() { // NOLINT: match stl API
			return ColumnLogicalIteratorInternal(list, physical, Size(), Size());
		}
	};
};

} // namespace goose
