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

#include <goose/common/types-import.h>
#include <goose/common/typedefs.h>
#include <goose/common/string.h>
#include <goose/common/vector.h>

namespace goose {

//! Information used to validate
struct ValidatorLine {
	ValidatorLine(idx_t start_pos_p, idx_t end_pos_p) : start_pos(start_pos_p), end_pos(end_pos_p) {
	}
	const idx_t start_pos;
	const idx_t end_pos;
};

struct ThreadLines {
	ThreadLines() {};
	//! Validate everything is as it should be, returns true if it's all good, false o.w.
	void Verify() const;

	void Insert(idx_t thread, ValidatorLine line_info);

	string Print() const;

private:
	map<idx_t, ValidatorLine> thread_lines;
	//! We allow up to 2 bytes of error margin (basically \r\n)
	static constexpr idx_t error_margin = 2;
};

//! The validator works by double-checking that threads started and ended in the right positions
struct CSVValidator {
	CSVValidator() {
	}
	//! Validate that all lines are good
	void Verify() const;

	//! Inserts line_info to a given thread index
	void Insert(idx_t thread, ValidatorLine line_info);

	string Print() const;

private:
	//! Thread lines for this file
	ThreadLines thread_lines;
};

} // namespace goose
