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

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <goose/common/operator/comparison_operators.h>
#include <goose/common/optional_idx.h>
#include <goose/parser/parsed_data/sample_options.h>

namespace goose {

class ExtraOperatorInfo {
public:
	ExtraOperatorInfo() : file_filters(""), sample_options(nullptr) {
	}
	ExtraOperatorInfo(ExtraOperatorInfo &&extra_info) noexcept
	    : file_filters(std::move(extra_info.file_filters)), sample_options(std::move(extra_info.sample_options)) {
		if (extra_info.total_files.IsValid()) {
			total_files = extra_info.total_files.GetIndex();
		}
		if (extra_info.filtered_files.IsValid()) {
			filtered_files = extra_info.filtered_files.GetIndex();
		}
	}
	ExtraOperatorInfo &operator=(ExtraOperatorInfo &&extra_info) noexcept {
		if (this != &extra_info) {
			file_filters = extra_info.file_filters;
			if (extra_info.total_files.IsValid()) {
				total_files = extra_info.total_files.GetIndex();
			}
			if (extra_info.filtered_files.IsValid()) {
				filtered_files = extra_info.filtered_files.GetIndex();
			}
			sample_options = std::move(extra_info.sample_options);
		}
		return *this;
	}

	bool operator==(const ExtraOperatorInfo &other) const {
		return file_filters == other.file_filters && total_files == other.total_files &&
		       filtered_files == other.filtered_files && sample_options == other.sample_options;
	}

	//! Filters that have been pushed down into the main file list
	string file_filters;
	//! Total size of file list
	optional_idx total_files;
	//! Size of file list after applying filters
	optional_idx filtered_files;
	//! Sample options that have been pushed down into the table scan
	unique_ptr<SampleOptions> sample_options;

	void Serialize(Serializer &serializer) const;
	static ExtraOperatorInfo Deserialize(Deserializer &deserializer);
};

} // namespace goose
