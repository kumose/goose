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

#include <goose/transaction/undo_buffer.h>
#include <goose/common/types/data_chunk.h>
#include <goose/common/types-import.h>
#include <goose/main/client_context.h>
#include <goose/transaction/commit_state.h>

namespace goose {

class DataTable;

struct DeleteInfo;
struct UpdateInfo;

class CleanupState {
public:
	explicit CleanupState(GooseTransaction &transaction, transaction_t lowest_active_transaction,
	                      ActiveTransactionState transaction_state);

public:
	void CleanupEntry(UndoFlags type, data_ptr_t data);

private:
	//! Lowest active transaction
	transaction_t lowest_active_transaction;
	ActiveTransactionState transaction_state;
	//! While cleaning up, we remove data from any delta indexes we added data to during the commit
	IndexDataRemover index_data_remover;

private:
	void CleanupDelete(DeleteInfo &info);
	void CleanupUpdate(UpdateInfo &info);
};

} // namespace goose
