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

#include <goose/common/constants.h>

namespace goose {

enum class TableScanType : uint8_t {
	//! Regular table scan: scan all tuples that are relevant for the current transaction
	TABLE_SCAN_REGULAR = 0,
	//! Scan all rows, including any deleted rows. Committed updates are merged in.
	TABLE_SCAN_COMMITTED_ROWS = 1,
	//! Scan all rows, including any deleted rows. Throws an exception if there are any uncommitted updates.
	TABLE_SCAN_COMMITTED_ROWS_DISALLOW_UPDATES = 2,
	//! Scan all rows, excluding any permanently deleted rows.
	//! Permanently deleted rows are rows which no transaction will ever need again.
	TABLE_SCAN_COMMITTED_ROWS_OMIT_PERMANENTLY_DELETED = 3,
	//! Scan the latest committed rows
	TABLE_SCAN_LATEST_COMMITTED_ROWS = 4
};

} // namespace goose
