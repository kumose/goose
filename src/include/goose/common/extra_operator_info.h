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
