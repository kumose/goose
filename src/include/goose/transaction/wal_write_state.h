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

namespace goose {
class CatalogEntry;
class DataChunk;
class GooseTransaction;
class WriteAheadLog;
class ClientContext;

struct DataTableInfo;
struct DeleteInfo;
struct UpdateInfo;

class WALWriteState {
public:
	explicit WALWriteState(GooseTransaction &transaction, WriteAheadLog &log,
	                       optional_ptr<StorageCommitState> commit_state);

public:
	void CommitEntry(UndoFlags type, data_ptr_t data);

private:
	void SwitchTable(DataTableInfo &table, UndoFlags new_op);

	void WriteCatalogEntry(CatalogEntry &entry, data_ptr_t extra_data);
	void WriteDelete(DeleteInfo &info);
	void WriteUpdate(UpdateInfo &info);

private:
	GooseTransaction &transaction;
	WriteAheadLog &log;
	optional_ptr<StorageCommitState> commit_state;

	optional_ptr<DataTableInfo> current_table_info;

	unique_ptr<DataChunk> delete_chunk;
	unique_ptr<DataChunk> update_chunk;
};

} // namespace goose
