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

#include <goose/function/compression_function.h>
#include <goose/function/function_set.h>

namespace goose {

struct ConstantFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
	static void FiltersNullValues(const LogicalType &type, const TableFilter &filter, bool &filters_nulls,
	                              bool &filters_valid_values, TableFilterState &filter_state);
};

struct UncompressedFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
};

struct RLEFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
};

struct BitpackingFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
};

struct DictionaryCompressionFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
};

struct DictFSSTCompressionFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
};

struct ChimpCompressionFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
};

struct PatasCompressionFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
};

struct AlpCompressionFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
};

struct AlpRDCompressionFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
};

struct FSSTFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
};

struct ZSTDFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(PhysicalType type);
};

struct RoaringCompressionFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
};

struct EmptyValidityCompressionFun {
	static CompressionFunction GetFunction(PhysicalType type);
	static bool TypeIsSupported(const PhysicalType physical_type);
};

} // namespace goose
