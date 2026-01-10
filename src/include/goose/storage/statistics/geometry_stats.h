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

#include <goose/common/common.h>
#include <goose/common/enums/expression_type.h>
#include <goose/common/enums/filter_propagate_result.h>
#include <goose/common/exception.h>
#include <goose/common/operator/comparison_operators.h>
#include <goose/common/types/hugeint.h>
#include <goose/common/array_ptr.h>
#include <goose/common/types/geometry.h>

namespace goose {
class BaseStatistics;
struct SelectionVector;

class GeometryTypeSet {
public:
	static constexpr auto VERT_TYPES = 4;
	static constexpr auto PART_TYPES = 8;

	static GeometryTypeSet Unknown() {
		GeometryTypeSet result;
		for (idx_t i = 0; i < VERT_TYPES; i++) {
			result.sets[i] = 0xFF;
		}
		return result;
	}
	static GeometryTypeSet Empty() {
		GeometryTypeSet result;
		for (idx_t i = 0; i < VERT_TYPES; i++) {
			result.sets[i] = 0;
		}
		return result;
	}

	bool IsEmpty() const {
		for (idx_t i = 0; i < VERT_TYPES; i++) {
			if (sets[i] != 0) {
				return false;
			}
		}
		return true;
	}

	bool IsUnknown() const {
		for (idx_t i = 0; i < VERT_TYPES; i++) {
			if (sets[i] != 0xFF) {
				return false;
			}
		}
		return true;
	}

	void Add(GeometryType geom_type, VertexType vert_type) {
		const auto vert_idx = static_cast<uint8_t>(vert_type);
		const auto geom_idx = static_cast<uint8_t>(geom_type);
		D_ASSERT(vert_idx < VERT_TYPES);
		D_ASSERT(geom_idx < PART_TYPES);
		sets[vert_idx] |= (1 << geom_idx);
	}

	void Merge(const GeometryTypeSet &other) {
		for (idx_t i = 0; i < VERT_TYPES; i++) {
			sets[i] |= other.sets[i];
		}
	}

	void Clear() {
		for (idx_t i = 0; i < VERT_TYPES; i++) {
			sets[i] = 0;
		}
	}

	void AddWKBType(int32_t wkb_type) {
		const auto vert_idx = static_cast<uint8_t>((wkb_type / 1000) % 10);
		const auto geom_idx = static_cast<uint8_t>(wkb_type % 1000);
		D_ASSERT(vert_idx < VERT_TYPES);
		D_ASSERT(geom_idx < PART_TYPES);
		sets[vert_idx] |= (1 << geom_idx);
	}

	vector<int32_t> ToWKBList() const {
		vector<int32_t> result;
		for (uint8_t vert_idx = 0; vert_idx < VERT_TYPES; vert_idx++) {
			for (uint8_t geom_idx = 1; geom_idx < PART_TYPES; geom_idx++) {
				if (sets[vert_idx] & (1 << geom_idx)) {
					result.push_back(geom_idx + vert_idx * 1000);
				}
			}
		}
		return result;
	}

	vector<string> ToString(bool snake_case) const;

	uint8_t sets[VERT_TYPES];
};

struct GeometryStatsData {
	GeometryTypeSet types;
	GeometryExtent extent;

	void SetEmpty() {
		types = GeometryTypeSet::Empty();
		extent = GeometryExtent::Empty();
	}

	void SetUnknown() {
		types = GeometryTypeSet::Unknown();
		extent = GeometryExtent::Unknown();
	}

	void Merge(const GeometryStatsData &other) {
		types.Merge(other.types);
		extent.Merge(other.extent);
	}

	void Update(const string_t &geom_blob) {
		// Parse type
		const auto type_info = Geometry::GetType(geom_blob);
		types.Add(type_info.first, type_info.second);

		// Update extent
		Geometry::GetExtent(geom_blob, extent);
	}
};

struct GeometryStats {
	//! Unknown statistics
	GOOSE_API static BaseStatistics CreateUnknown(LogicalType type);
	//! Empty statistics
	GOOSE_API static BaseStatistics CreateEmpty(LogicalType type);

	GOOSE_API static void Serialize(const BaseStatistics &stats, Serializer &serializer);
	GOOSE_API static void Deserialize(Deserializer &deserializer, BaseStatistics &base);

	GOOSE_API static string ToString(const BaseStatistics &stats);

	GOOSE_API static void Update(BaseStatistics &stats, const string_t &value);
	GOOSE_API static void Merge(BaseStatistics &stats, const BaseStatistics &other);
	GOOSE_API static void Verify(const BaseStatistics &stats, Vector &vector, const SelectionVector &sel, idx_t count);

	//! Check if a spatial predicate check with a constant could possibly be satisfied by rows given the statistics
	GOOSE_API static FilterPropagateResult CheckZonemap(const BaseStatistics &stats,
	                                                     const unique_ptr<Expression> &expr);

	GOOSE_API static GeometryExtent &GetExtent(BaseStatistics &stats);
	GOOSE_API static const GeometryExtent &GetExtent(const BaseStatistics &stats);
	GOOSE_API static GeometryTypeSet &GetTypes(BaseStatistics &stats);
	GOOSE_API static const GeometryTypeSet &GetTypes(const BaseStatistics &stats);

private:
	static GeometryStatsData &GetDataUnsafe(BaseStatistics &stats);
	static const GeometryStatsData &GetDataUnsafe(const BaseStatistics &stats);
};

} // namespace goose
