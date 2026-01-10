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
