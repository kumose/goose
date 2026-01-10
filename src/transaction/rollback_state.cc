#include <goose/transaction/rollback_state.h>
#include <goose/transaction/append_info.h>
#include <goose/transaction/delete_info.h>
#include <goose/transaction/update_info.h>

#include <goose/storage/table/chunk_info.h>

#include <goose/catalog/catalog_entry.h>
#include <goose/catalog/catalog_set.h>
#include <goose/storage/data_table.h>
#include <goose/storage/table/update_segment.h>
#include <goose/storage/table/row_version_manager.h>
#include <goose/main/attached_database.h>

namespace goose {

RollbackState::RollbackState(GooseTransaction &transaction_p) : transaction(transaction_p) {
}

void RollbackState::RollbackEntry(UndoFlags type, data_ptr_t data) {
	switch (type) {
	case UndoFlags::CATALOG_ENTRY: {
		// Load and undo the catalog entry.
		auto catalog_entry = Load<CatalogEntry *>(data);
		D_ASSERT(catalog_entry->set);
		catalog_entry->set->Undo(*catalog_entry);
		break;
	}
	case UndoFlags::INSERT_TUPLE: {
		auto info = reinterpret_cast<AppendInfo *>(data);
		// revert the append in the base table
		info->table->RevertAppend(transaction, info->start_row, info->count);
		break;
	}
	case UndoFlags::DELETE_TUPLE: {
		auto info = reinterpret_cast<DeleteInfo *>(data);
		// reset the deleted flag on rollback
		info->version_info->CommitDelete(info->vector_idx, NOT_DELETED_ID, *info);
		break;
	}
	case UndoFlags::UPDATE_TUPLE: {
		auto info = reinterpret_cast<UpdateInfo *>(data);
		info->segment->RollbackUpdate(*info);
		break;
	}
	case UndoFlags::ATTACHED_DATABASE: {
		auto db = Load<AttachedDatabase *>(data);
		auto &db_manager = DatabaseManager::Get(db->GetDatabase());
		db_manager.DetachInternal(db->name);
		break;
	}
	case UndoFlags::SEQUENCE_VALUE:
		break;
	default: // LCOV_EXCL_START
		D_ASSERT(type == UndoFlags::EMPTY_ENTRY);
		break;
	} // LCOV_EXCL_STOP
}

} // namespace goose
