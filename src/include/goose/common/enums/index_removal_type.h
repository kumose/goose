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

namespace goose {

enum class IndexRemovalType {
	//! Remove from main index, insert into deleted_rows_in_use
	MAIN_INDEX,
	//! Remove from main index only
	MAIN_INDEX_ONLY,
	//! Revert MAIN_INDEX, i.e. append to main index and remove from deleted_rows_in_use
	REVERT_MAIN_INDEX,
	//! Revert MAIN_INDEX_ONLY, i.e. append to main index
	REVERT_MAIN_INDEX_ONLY,
	//! Remove from deleted_rows_in_use
	DELETED_ROWS_IN_USE
};

} // namespace goose
