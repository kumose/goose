#include <goose/storage/index.h>
#include <goose/common/radix.h>
#include <goose/common/serializer/serializer.h>

namespace goose {

Index::Index(const vector<column_t> &column_ids, TableIOManager &table_io_manager, AttachedDatabase &db)

    : column_ids(column_ids), table_io_manager(table_io_manager), db(db) {
	// create the column id set
	column_id_set.insert(column_ids.begin(), column_ids.end());
}

} // namespace goose
