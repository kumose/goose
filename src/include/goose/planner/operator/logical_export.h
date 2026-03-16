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
