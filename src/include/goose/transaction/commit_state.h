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
#include <goose/common/vector_size.h>
#include <goose/common/enums/index_removal_type.h>
#include <goose/main/client_context.h>

namespace goose {
class CatalogEntry;
class DataChunk;
class GooseTransaction;
class WriteAheadLog;
class ClientContext;

struct DataTableInfo;
class DataTable;
struct DeleteInfo;
struct UpdateInfo;

enum class CommitMode { COMMIT, REVERT_COMMIT };

struct IndexDataRemover {
public:
	explicit IndexDataRemover(GooseTransaction &transaction, QueryContext context, IndexRemovalType removal_type);

	void PushDelete(DeleteInfo &info);
	void Verify();

private:
	void Flush(DataTable &table, row_t *row_numbers, idx_t count);

private:
	GooseTransaction &transaction;
	// data for index cleanup
	QueryContext context;
	//! While committing, we remove data from any indexes that was deleted
	IndexRemovalType removal_type;
	DataChunk chunk;
	//! Debug mode only - list of indexes to verify
	reference_map_t<DataTable, shared_ptr<DataTableInfo>> verify_indexes;
};

class CommitState {
public:
	explicit CommitState(GooseTransaction &transaction, transaction_t commit_id,
	                     ActiveTransactionState transaction_state, CommitMode commit_mode);

public:
	void CommitEntry(UndoFlags type, data_ptr_t data);
	void RevertCommit(UndoFlags type, data_ptr_t data);
	void Flush();
	void Verify();
	static IndexRemovalType GetIndexRemovalType(ActiveTransactionState transaction_state, CommitMode commit_mode);

private:
	void CommitEntryDrop(CatalogEntry &entry, data_ptr_t extra_data);
	void CommitDelete(DeleteInfo &info);

private:
	GooseTransaction &transaction;
	transaction_t commit_id;
	IndexDataRemover index_data_remover;
};

} // namespace goose
