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

#include <goose/execution/operator/csv_scanner/csv_reader_options.h>
#include <goose/main/relation/table_function_relation.h>
#include <goose/common/shared_ptr.h>
#include <goose/common/case_insensitive_map.h>

namespace goose {

class ReadCSVRelation : public TableFunctionRelation {
public:
	ReadCSVRelation(const shared_ptr<ClientContext> &context, const vector<string> &csv_files,
	                named_parameter_map_t &&options, string alias = string());

	string alias;

protected:
	void InitializeAlias(const vector<string> &input);

public:
	string GetAlias() override;
};

} // namespace goose
