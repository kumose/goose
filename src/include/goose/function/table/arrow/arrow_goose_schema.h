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

#include <goose/common/types/validity_mask.h>

#include <goose/common/types.h>
#include <goose/common/types-import.h>
#include <goose/common/unique_ptr.h>
#include <goose/function/table/arrow/arrow_type_info.h>
#include <goose/common/arrow/arrow.h>

namespace goose {
struct DBConfig;

struct ArrowArrayScanState;

typedef void (*cast_arrow_goose_t)(ClientContext &context, Vector &source, Vector &result, idx_t count);

typedef void (*cast_goose_arrow_t)(ClientContext &context, Vector &source, Vector &result, idx_t count);

class ArrowTypeExtensionData {
public:
	explicit ArrowTypeExtensionData(const LogicalType &goose_type, const LogicalType &internal_type_p,
	                                cast_arrow_goose_t arrow_to_goose = nullptr,
	                                cast_goose_arrow_t goose_to_arrow = nullptr)
	    : arrow_to_goose(arrow_to_goose), goose_to_arrow(goose_to_arrow), goose_type(goose_type),
	      internal_type(internal_type_p) {
	}

	explicit ArrowTypeExtensionData(const LogicalType &goose_type)
	    : goose_type(goose_type), internal_type(goose_type) {
	}

	//! (Optional) Callback to function that converts an Arrow Array to a Goose Vector
	cast_arrow_goose_t arrow_to_goose = nullptr;
	//! (Optional) Callback to function that converts a Goose Vector to an Arrow Array
	cast_goose_arrow_t goose_to_arrow = nullptr;

	LogicalType GetInternalType() const;

	//! This function returns possible extension types to given Goose types
	static unordered_map<idx_t, const shared_ptr<ArrowTypeExtensionData>>
	GetExtensionTypes(ClientContext &context, const vector<LogicalType> &goose_types);
	LogicalType GetGooseType() const;

private:
	//! Original Goose Type (e.g., UHUGEINT)
	LogicalType goose_type;
	//! Internal type is a type that refers to the produced goose vector when consuming the arrow format
	LogicalType internal_type;
};

class ArrowType {
public:
	//! From a Goose type
	explicit ArrowType(LogicalType type_p, unique_ptr<ArrowTypeInfo> type_info = nullptr)
	    : type(std::move(type_p)), type_info(std::move(type_info)) {
	}
	explicit ArrowType(string error_message_p, bool not_implemented_p = false)
	    : type(LogicalTypeId::INVALID), type_info(nullptr), error_message(std::move(error_message_p)),
	      not_implemented(not_implemented_p) {
	}

public:
	LogicalType GetGooseType(bool use_dictionary = false) const;

	void SetDictionary(unique_ptr<ArrowType> dictionary);
	bool HasDictionary() const;
	const ArrowType &GetDictionary() const;

	bool RunEndEncoded() const;
	void SetRunEndEncoded();

	template <class T>
	const T &GetTypeInfo() const {
		return type_info->Cast<T>();
	}
	void ThrowIfInvalid() const;

	static unique_ptr<ArrowType> GetTypeFromFormat(string &format);
	static unique_ptr<ArrowType> GetTypeFromFormat(DBConfig &config, ArrowSchema &schema, string &format);

	static unique_ptr<ArrowType> GetTypeFromSchema(DBConfig &config, ArrowSchema &schema);

	static unique_ptr<ArrowType> CreateListType(DBConfig &config, ArrowSchema &child, ArrowVariableSizeType size_type,
	                                            bool view);

	static unique_ptr<ArrowType> GetArrowLogicalType(DBConfig &config, ArrowSchema &schema);

	bool HasExtension() const;

	ArrowArrayPhysicalType GetPhysicalType() const;

	//! The Arrow Type Extension data, if any
	shared_ptr<ArrowTypeExtensionData> extension_data;

protected:
	LogicalType type;
	//! Hold the optional type if the array is a dictionary
	unique_ptr<ArrowType> dictionary_type;
	//! Is run-end-encoded
	bool run_end_encoded = false;
	unique_ptr<ArrowTypeInfo> type_info;
	//! Error message in case of an invalid type (i.e., from an unsupported extension)
	string error_message;
	//! In case of an error do we throw not implemented?
	bool not_implemented = false;
};

using arrow_column_map_t = unordered_map<idx_t, shared_ptr<ArrowType>>;

struct ArrowTableSchema {
public:
	void AddColumn(idx_t index, shared_ptr<ArrowType> type, const string &name);
	const arrow_column_map_t &GetColumns() const;
	vector<LogicalType> &GetTypes();
	vector<string> &GetNames();

private:
	arrow_column_map_t arrow_convert_data;
	vector<LogicalType> types;
	vector<string> column_names;
};

} // namespace goose
