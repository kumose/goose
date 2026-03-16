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

#include "shell_state.h"
#include "shell_highlight.h"

namespace goose_shell {
struct ShellState;
struct RenderingResultIterator;

struct ResultMetadata {
	explicit ResultMetadata(goose::QueryResult &result);

	vector<string> column_names;
	vector<goose::LogicalType> types;
	vector<string> type_names;

	idx_t ColumnCount() const {
		return column_names.size();
	}
};

struct RowData {
	vector<goose::string_t> data;
	vector<bool> is_null;
	idx_t row_index = 0;
};

struct RenderingQueryResult {
	RenderingQueryResult(goose::QueryResult &result, ShellRenderer &renderer);

	goose::QueryResult &result;
	ShellRenderer &renderer;
	ResultMetadata metadata;
	vector<unique_ptr<goose::DataChunk>> chunks;
	bool exhausted_result = false;
	idx_t loaded_row_count = 0;

	idx_t ColumnCount() const {
		return metadata.ColumnCount();
	}
	bool TryConvertChunk();

public:
	RenderingResultIterator begin(); // NOLINT: match stl API
	RenderingResultIterator end();   // NOLINT: match stl API
};

enum class TextAlignment { CENTER, LEFT, RIGHT };

struct PrintStream {
public:
	explicit PrintStream(ShellState &state);
	virtual ~PrintStream() = default;

	virtual void Print(const string &str) {
		state.Print(str);
	}
	virtual void Print(goose::string_t str) {
		state.Print(str);
	}
	virtual void Print(const char *str) {
		state.Print(str);
	}
	virtual void SetBinaryMode() {
		state.SetBinaryMode();
	}
	virtual void SetTextMode() {
		state.SetTextMode();
	}
	virtual bool SupportsHighlight() {
		return true;
	}

	void RenderAlignedValue(const string &str, idx_t width, TextAlignment alignment = TextAlignment::CENTER);
	void RenderAlignedValue(const char *str, idx_t str_len, idx_t width,
	                        TextAlignment alignment = TextAlignment::CENTER);
	void RenderAlignedValue(goose::string_t str, idx_t width, TextAlignment alignment = TextAlignment::CENTER);
	void PrintDashes(idx_t N);
	void OutputQuotedIdentifier(const string &str);
	void OutputQuotedString(const string &str);

public:
	ShellState &state;
};

class ShellRenderer {
public:
	explicit ShellRenderer(ShellState &state);
	virtual ~ShellRenderer() = default;

	ShellState &state;
	bool show_header;
	string col_sep;
	string row_sep;

public:
	virtual SuccessState RenderQueryResult(PrintStream &out, ShellState &state, RenderingQueryResult &result);
	virtual void RemoveRenderLimits() {
	}
	virtual void Analyze(RenderingQueryResult &result);
	virtual void RenderHeader(PrintStream &out, ResultMetadata &result);
	virtual void RenderRow(PrintStream &out, ResultMetadata &result, RowData &row);
	virtual void RenderFooter(PrintStream &out, ResultMetadata &result);
	virtual const char *NullValue();
	virtual bool RequireMaterializedResult() const = 0;
	virtual bool ShouldUsePager(RenderingQueryResult &result, PagerMode global_mode) = 0;
	virtual unique_ptr<goose::DataChunk> ConvertChunk(goose::DataChunk &chunk);
	virtual bool HasConvertValue() {
		return false;
	}
	virtual bool ShouldConvertValue(const char *value, idx_t str_len) {
		return false;
	}
	virtual string ConvertValue(const char *value, idx_t str_len);
};

class ColumnRenderer : public ShellRenderer {
public:
	explicit ColumnRenderer(ShellState &state);

	void Analyze(RenderingQueryResult &result) override;
	void RenderRow(PrintStream &out, ResultMetadata &result, RowData &row) override;

	virtual const char *GetColumnSeparator() = 0;
	virtual const char *GetRowSeparator() = 0;
	virtual const char *GetRowStart() {
		return nullptr;
	}
	bool RequireMaterializedResult() const override {
		return true;
	}
	bool ShouldUsePager(RenderingQueryResult &result, PagerMode global_mode) override;

protected:
	vector<idx_t> column_width;
	vector<bool> right_align;
};

class RowRenderer : public ShellRenderer {
public:
	explicit RowRenderer(ShellState &state);

public:
	void RenderHeader(PrintStream &out, ResultMetadata &result) override;
	bool RequireMaterializedResult() const override {
		return false;
	}
	bool ShouldUsePager(RenderingQueryResult &result, PagerMode global_mode) override;
};

class ShellLogStorage : public goose::LogStorage {
public:
	explicit ShellLogStorage(ShellState &state) : shell_highlight(state) {};

	~ShellLogStorage() override = default;

	const string GetStorageName() override {
		return "ShellLogStorage";
	}

protected:
	void WriteLogEntry(goose::timestamp_t timestamp, goose::LogLevel level, const string &log_type,
	                   const string &log_message, const goose::RegisteredLoggingContext &context) override;
	void WriteLogEntries(goose::DataChunk &chunk, const goose::RegisteredLoggingContext &context) override {};
	void FlushAll() override {};
	void Flush(goose::LoggingTargetTable table) override {};
	bool IsEnabled(goose::LoggingTargetTable table) override {
		return true;
	};

private:
	ShellHighlight shell_highlight;
};

} // namespace goose_shell
