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

#include "json_common.h"

namespace goose {

struct JSONStructureDescription;
struct DateFormatMap;
struct StrpTimeFormat;
class MutableDateFormatMap;

struct JSONStructureNode {
public:
	JSONStructureNode();
	JSONStructureNode(const char *key_ptr, const size_t key_len);
	JSONStructureNode(yyjson_val *key_p, yyjson_val *val_p, bool ignore_errors);

	//! Disable copy constructors
	JSONStructureNode(const JSONStructureNode &other) = delete;
	JSONStructureNode &operator=(const JSONStructureNode &) = delete;
	//! Enable move constructors
	JSONStructureNode(JSONStructureNode &&other) noexcept;
	JSONStructureNode &operator=(JSONStructureNode &&) noexcept;

	JSONStructureDescription &GetOrCreateDescription(LogicalTypeId type);

	bool ContainsVarchar() const;
	void InitializeCandidateTypes(idx_t max_depth, bool convert_strings_to_integers, idx_t depth = 0);
	void RefineCandidateTypes(yyjson_val *vals[], idx_t val_count, Vector &string_vector, ArenaAllocator &allocator,
	                          MutableDateFormatMap &date_format_map);

private:
	void RefineCandidateTypesArray(yyjson_val *vals[], idx_t val_count, Vector &string_vector,
	                               ArenaAllocator &allocator, MutableDateFormatMap &date_format_map);
	void RefineCandidateTypesObject(yyjson_val *vals[], idx_t val_count, Vector &string_vector,
	                                ArenaAllocator &allocator, MutableDateFormatMap &date_format_map);
	void RefineCandidateTypesString(yyjson_val *vals[], idx_t val_count, Vector &string_vector,
	                                MutableDateFormatMap &date_format_map);
	void EliminateCandidateTypes(idx_t vec_count, Vector &string_vector, MutableDateFormatMap &date_format_map);
	bool EliminateCandidateFormats(idx_t vec_count, Vector &string_vector, const Vector &result_vector,
	                               MutableDateFormatMap &date_format_map);

public:
	unique_ptr<string> key;
	bool initialized = false;
	vector<JSONStructureDescription> descriptions;
	idx_t count;
	idx_t null_count;
};

struct JSONStructureDescription {
public:
	explicit JSONStructureDescription(LogicalTypeId type_p);
	//! Disable copy constructors
	JSONStructureDescription(const JSONStructureDescription &other) = delete;
	JSONStructureDescription &operator=(const JSONStructureDescription &) = delete;
	//! Enable move constructors
	JSONStructureDescription(JSONStructureDescription &&other) noexcept;
	JSONStructureDescription &operator=(JSONStructureDescription &&) noexcept;

	JSONStructureNode &GetOrCreateChild();
	JSONStructureNode &GetOrCreateChild(const char *key_ptr, size_t key_size);
	JSONStructureNode &GetOrCreateChild(yyjson_val *key, yyjson_val *val, bool ignore_errors);

public:
	//! Type of this description
	LogicalTypeId type = LogicalTypeId::INVALID;

	//! Map to children and children
	json_key_map_t<idx_t> key_map;
	vector<JSONStructureNode> children;

	//! Candidate types (if auto-detecting and type == LogicalTypeId::VARCHAR)
	vector<LogicalTypeId> candidate_types;
};

struct JSONStructure {
public:
	static void ExtractStructure(yyjson_val *val, JSONStructureNode &node, bool ignore_errors);
	static void MergeNodes(JSONStructureNode &merged, const JSONStructureNode &node);
	static LogicalType StructureToType(ClientContext &context, const JSONStructureNode &node, idx_t max_depth,
	                                   double field_appearance_threshold, idx_t map_inference_threshold,
	                                   idx_t depth = 0, const LogicalType &null_type = LogicalType::JSON());
};

} // namespace goose
