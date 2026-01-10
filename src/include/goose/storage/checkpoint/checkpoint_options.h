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

#include <goose/common/enums/checkpoint_type.h>

namespace goose {

struct CheckpointOptions {
	CheckpointOptions()
	    : wal_action(CheckpointWALAction::DONT_DELETE_WAL), action(CheckpointAction::CHECKPOINT_IF_REQUIRED),
	      type(CheckpointType::FULL_CHECKPOINT), transaction_id(MAX_TRANSACTION_ID) {
	}

	CheckpointWALAction wal_action;
	CheckpointAction action;
	CheckpointType type;
	transaction_t transaction_id;
};

} // namespace goose
