#include <goose/storage/table/persistent_table_data.h>
#include <goose/storage/statistics/base_statistics.h>

namespace goose {

PersistentTableData::PersistentTableData(idx_t column_count) : total_rows(0), row_group_count(0) {
}

PersistentTableData::~PersistentTableData() {
}

} // namespace goose
