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

#include <goose/parser/parsed_data/copy_info.h>
#include <goose/parser/parsed_data/exported_table_data.h>
#include <goose/planner/logical_operator.h>
#include <goose/function/copy_function.h>

namespace goose {

class LogicalExport : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_EXPORT;

public:
	LogicalExport(CopyFunction function, unique_ptr<CopyInfo> copy_info, unique_ptr<BoundExportData> exported_tables);

	unique_ptr<CopyInfo> copy_info;
	CopyFunction function;
	unique_ptr<BoundExportData> exported_tables;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

protected:
	LogicalExport(ClientContext &context, unique_ptr<ParseInfo> copy_info, unique_ptr<ParseInfo> exported_tables);

	void ResolveTypes() override {
		types.emplace_back(LogicalType::BOOLEAN);
	}

	CopyFunction GetCopyFunction(ClientContext &context, CopyInfo &info);
};

} // namespace goose
