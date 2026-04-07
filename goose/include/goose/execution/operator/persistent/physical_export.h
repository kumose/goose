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

#include <utility>

#include <goose/execution/physical_operator.h>
#include <goose/function/copy_function.h>
#include <goose/parser/parsed_data/copy_info.h>
#include <goose/parser/parsed_data/exported_table_data.h>
#include <goose/catalog/catalog_entry_map.h>

namespace goose {

struct ExportEntries {
	vector<reference<CatalogEntry>> schemas;
	vector<reference<CatalogEntry>> custom_types;
	vector<reference<CatalogEntry>> sequences;
	vector<reference<CatalogEntry>> tables;
	vector<reference<CatalogEntry>> views;
	vector<reference<CatalogEntry>> indexes;
	vector<reference<CatalogEntry>> macros;
};

//! Parse a file from disk using a specified copy function and return the set of chunks retrieved from the file
class PhysicalExport : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::EXPORT;

public:
	PhysicalExport(PhysicalPlan &physical_plan, vector<LogicalType> types, CopyFunction function,
	               unique_ptr<CopyInfo> info, idx_t estimated_cardinality, unique_ptr<BoundExportData> exported_tables);

	//! The copy function to use to read the file
	CopyFunction function;
	//! The binding info containing the set of options for reading the file
	unique_ptr<CopyInfo> info;
	//! The table info for each table that will be exported
	unique_ptr<BoundExportData> exported_tables;

public:
	// Source interface
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

	static void ExtractEntries(ClientContext &context, vector<reference<SchemaCatalogEntry>> &schemas,
	                           ExportEntries &result);
	static catalog_entry_vector_t GetNaiveExportOrder(ClientContext &context, Catalog &catalog);

public:
	// Sink interface
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;

	bool ParallelSink() const override {
		return true;
	}
	bool IsSink() const override {
		return true;
	}

public:
	void BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) override;
	vector<const_reference<PhysicalOperator>> GetSources() const override;
};

} // namespace goose
