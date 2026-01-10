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

namespace tpcds {

struct DSDGenWrapper {
	//! Create the TPC-DS tables in the given schema with the given suffix
	static void CreateTPCDSSchema(goose::ClientContext &context, std::string catalog, std::string schema, std::string suffix, bool keys,
	                              bool overwrite);
	//! Generate the TPC-DS data of the given scale factor
	static void DSDGen(double scale, goose::ClientContext &context, std::string catalog, std::string schema, std::string suffix);

	static uint32_t QueriesCount();
	//! Gets the specified TPC-DS Query number as a string
	static std::string GetQuery(int query);
	//! Returns the CSV answer of a TPC-DS query
	static std::string GetAnswer(double sf, int query);
};

} // namespace tpcds
