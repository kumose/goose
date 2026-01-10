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

#include <goose/common/types.h>

namespace goose {

struct TypeVisitor {
	template <class F>
	static bool Contains(const LogicalType &type, F &&predicate);

	static bool Contains(const LogicalType &type, LogicalTypeId type_id);

	template <class F>
	static LogicalType VisitReplace(const LogicalType &type, F &&func);
};

template <class F>
inline LogicalType TypeVisitor::VisitReplace(const LogicalType &type, F &&func) {
	switch (type.id()) {
	case LogicalTypeId::STRUCT: {
		if (!type.AuxInfo()) {
			return func(type);
		}
		auto children = StructType::GetChildTypes(type);
		for (auto &child : children) {
			child.second = VisitReplace(child.second, func);
		}
		return func(LogicalType::STRUCT(children));
	}
	case LogicalTypeId::UNION: {
		if (!type.AuxInfo()) {
			return func(type);
		}
		auto children = UnionType::CopyMemberTypes(type);
		for (auto &child : children) {
			child.second = VisitReplace(child.second, func);
		}
		return func(LogicalType::UNION(children));
	}
	case LogicalTypeId::LIST: {
		if (!type.AuxInfo()) {
			return func(type);
		}
		const auto &child = ListType::GetChildType(type);
		return func(LogicalType::LIST(VisitReplace(child, func)));
	}
	case LogicalTypeId::ARRAY: {
		if (!type.AuxInfo()) {
			return func(type);
		}
		const auto &child = ArrayType::GetChildType(type);
		return func(LogicalType::ARRAY(VisitReplace(child, func), ArrayType::GetSize(type)));
	}
	case LogicalTypeId::MAP: {
		if (!type.AuxInfo()) {
			return func(type);
		}
		const auto &key = MapType::KeyType(type);
		const auto &value = MapType::ValueType(type);
		return func(LogicalType::MAP(VisitReplace(key, func), VisitReplace(value, func)));
	}
	default:
		return func(type);
	}
}

template <class F>
inline bool TypeVisitor::Contains(const LogicalType &type, F &&predicate) {
	if (predicate(type)) {
		return true;
	}
	switch (type.id()) {
	case LogicalTypeId::STRUCT: {
		if (!type.AuxInfo()) {
			return false;
		}
		for (const auto &child : StructType::GetChildTypes(type)) {
			if (Contains(child.second, predicate)) {
				return true;
			}
		}
		return false;
	}
	case LogicalTypeId::UNION:
		if (!type.AuxInfo()) {
			return false;
		}
		for (idx_t i = 0; i < UnionType::GetMemberCount(type); i++) {
			if (Contains(UnionType::GetMemberType(type, i), predicate)) {
				return true;
			}
		}
		return false;
	case LogicalTypeId::LIST:
		if (!type.AuxInfo()) {
			return false;
		}
		return Contains(ListType::GetChildType(type), predicate);
	case LogicalTypeId::ARRAY:
		if (!type.AuxInfo()) {
			return false;
		}
		return Contains(ArrayType::GetChildType(type), predicate);
	case LogicalTypeId::MAP:
		if (!type.AuxInfo()) {
			return false;
		}
		return Contains(MapType::KeyType(type), predicate) || Contains(MapType::ValueType(type), predicate);
	default:
		return false;
	}
}

inline bool TypeVisitor::Contains(const LogicalType &type, LogicalTypeId type_id) {
	return Contains(type, [&](const LogicalType &ty) { return ty.id() == type_id; });
}

} // namespace goose
