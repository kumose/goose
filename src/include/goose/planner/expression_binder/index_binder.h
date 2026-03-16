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

#include <goose/catalog/catalog_entry/table_catalog_entry.h>
#include <goose/common/types-import.h>
#include <goose/execution/index/bound_index.h>
#include <goose/execution/index/unbound_index.h>
#include <goose/parser/parsed_data/create_index_info.h>
#include <goose/planner/expression_binder.h>

namespace goose {

class BoundColumnRefExpression;

//! The IndexBinder binds indexes and expressions within index statements.
class IndexBinder : public ExpressionBinder {
public:
	IndexBinder(Binder &binder, ClientContext &context, optional_ptr<TableCatalogEntry> table = nullptr,
	            optional_ptr<CreateIndexInfo> info = nullptr);

	unique_ptr<BoundIndex> BindIndex(const UnboundIndex &index);
	unique_ptr<LogicalOperator> BindCreateIndex(ClientContext &context, unique_ptr<CreateIndexInfo> create_index_info,
	                                            TableCatalogEntry &table_entry, unique_ptr<LogicalOperator> plan,
	                                            unique_ptr<AlterTableInfo> alter_table_info);

	static void InitCreateIndexInfo(LogicalGet &get, CreateIndexInfo &info, const string &schema);

protected:
	BindResult BindExpression(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth,
	                          bool root_expression = false) override;
	string UnsupportedAggregateMessage() override;

private:
	// Only for WAL replay.
	optional_ptr<TableCatalogEntry> table;
	optional_ptr<CreateIndexInfo> info;
};

} // namespace goose
