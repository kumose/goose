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
