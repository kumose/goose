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
