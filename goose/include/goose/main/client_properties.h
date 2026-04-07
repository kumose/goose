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

#include <goose/common/string.h>
#include <goose/common/types.h>
#include <goose/common/enums/arrow_format_version.h>

namespace goose {

//! A set of properties from the client context that can be used to interpret the query result
struct ClientProperties {
	ClientProperties(string time_zone_p, const ArrowOffsetSize arrow_offset_size_p, const bool arrow_use_list_view_p,
	                 const bool produce_arrow_string_view_p, const bool lossless_conversion,
	                 const ArrowFormatVersion arrow_output_version, const optional_ptr<ClientContext> client_context)
	    : time_zone(std::move(time_zone_p)), arrow_offset_size(arrow_offset_size_p),
	      arrow_use_list_view(arrow_use_list_view_p), produce_arrow_string_view(produce_arrow_string_view_p),
	      arrow_lossless_conversion(lossless_conversion), arrow_output_version(arrow_output_version),
	      client_context(client_context) {
	}
	ClientProperties() {};

	string time_zone = "UTC";
	ArrowOffsetSize arrow_offset_size = ArrowOffsetSize::REGULAR;
	bool arrow_use_list_view = false;
	bool produce_arrow_string_view = false;
	bool arrow_lossless_conversion = false;
	ArrowFormatVersion arrow_output_version = ArrowFormatVersion::V1_0;
	optional_ptr<ClientContext> client_context;
};
} // namespace goose
