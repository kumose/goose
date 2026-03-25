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

#include <goose/function/scalar_function.h>
#include <goose/function/function_set.h>
#include <goose/function/built_in_functions.h>
#include <goose/common/types/variant.h>
#include <goose/common/owning_string_map.h>

namespace goose {

struct VariantNestedDataCollectionResult {
public:
	VariantNestedDataCollectionResult() : success(true) {
	}
	explicit VariantNestedDataCollectionResult(VariantLogicalType wrong_type) : success(false), wrong_type(wrong_type) {
	}

public:
	bool success;
	//! If success is false, the type that was encountered that caused the collection failure
	VariantLogicalType wrong_type;
};

struct VariantChildDataCollectionResult {
public:
	enum class Type : uint8_t { SUCCESS, INDEX_ZERO, COMPONENT_NOT_FOUND };

public:
	VariantChildDataCollectionResult() : type(Type::SUCCESS) {
	}

public:
	static VariantChildDataCollectionResult IndexZero() {
		return VariantChildDataCollectionResult(Type::INDEX_ZERO);
	}
	static VariantChildDataCollectionResult NotFound(idx_t nested_index) {
		return VariantChildDataCollectionResult(Type::COMPONENT_NOT_FOUND, nested_index);
	}

public:
	bool Success() const {
		return type == Type::SUCCESS;
	}

private:
	explicit VariantChildDataCollectionResult(Type type, idx_t index = DConstants::INVALID_INDEX)
	    : type(type), nested_data_index(index) {
	}

public:
	Type type;
	idx_t nested_data_index;
};

struct VariantUtils {
	GOOSE_API static bool IsNestedType(const UnifiedVariantVectorData &variant, idx_t row, uint32_t value_index);
	GOOSE_API static VariantDecimalData DecodeDecimalData(const UnifiedVariantVectorData &variant, idx_t row,
	                                                       uint32_t value_index);
	GOOSE_API static VariantNestedData DecodeNestedData(const UnifiedVariantVectorData &variant, idx_t row,
	                                                     uint32_t value_index);
	GOOSE_API static string_t DecodeStringData(const UnifiedVariantVectorData &variant, idx_t row,
	                                            uint32_t value_index);
	GOOSE_API static vector<string> GetObjectKeys(const UnifiedVariantVectorData &variant, idx_t row,
	                                               const VariantNestedData &nested_data);
	GOOSE_API static void FindChildValues(const UnifiedVariantVectorData &variant,
	                                       const VariantPathComponent &component,
	                                       optional_ptr<const SelectionVector> sel, SelectionVector &res,
	                                       ValidityMask &res_validity, VariantNestedData *nested_data, idx_t count);
	GOOSE_API static VariantNestedDataCollectionResult
	CollectNestedData(const UnifiedVariantVectorData &variant, VariantLogicalType expected_type,
	                  const SelectionVector &sel, idx_t count, optional_idx row, idx_t offset,
	                  VariantNestedData *child_data, ValidityMask &validity);
	GOOSE_API static vector<uint32_t> ValueIsNull(const UnifiedVariantVectorData &variant, const SelectionVector &sel,
	                                               idx_t count, optional_idx row);
	GOOSE_API static Value ConvertVariantToValue(const UnifiedVariantVectorData &variant, idx_t row,
	                                              uint32_t values_idx);
	GOOSE_API static bool Verify(Vector &variant, const SelectionVector &sel_p, idx_t count);
	GOOSE_API static void FinalizeVariantKeys(Vector &variant, OrderedOwningStringMap<uint32_t> &dictionary,
	                                           SelectionVector &sel, idx_t sel_size);
};

} // namespace goose
