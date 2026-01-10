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

#include <goose/goose.h>
#include <goose/catalog/catalog.h>
#include <goose/common/types/data_chunk.h>

namespace goose {
class ClientContext;
}

namespace tpch {

struct DBGenWrapper {
	//! Create the TPC-H tables in the given catalog / schema with the given suffix
	static void CreateTPCHSchema(goose::ClientContext &context, std::string catalog, std::string schema,
	                             std::string suffix);
	//! Load the TPC-H data of the given scale factor
	static void LoadTPCHData(goose::ClientContext &context, double sf, std::string catalog, std::string schema,
	                         std::string suffix, int children, int step);

	//! Gets the specified TPC-H Query number as a string
	static std::string GetQuery(int query);
	//! Returns the CSV answer of a TPC-H query
	static std::string GetAnswer(double sf, int query);
};

} // namespace tpch
