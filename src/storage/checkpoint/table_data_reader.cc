#include <goose/storage/checkpoint/table_data_reader.h>
#include <goose/storage/metadata/metadata_reader.h>
#include <goose/common/types/null_value.h>
#include <goose/common/serializer/binary_deserializer.h>
#include <goose/catalog/catalog_entry/table_catalog_entry.h>

#include <goose/planner/parsed_data/bound_create_table_info.h>

#include <goose/main/database.h>

namespace goose {

TableDataReader::TableDataReader(MetadataReader &reader, BoundCreateTableInfo &info, MetaBlockPointer table_pointer)
    : reader(reader), info(info) {
	info.data = make_uniq<PersistentTableData>(info.Base().columns.LogicalColumnCount());
	info.data->base_table_pointer = table_pointer;
}

void TableDataReader::ReadTableData() {
	auto &columns = info.Base().columns;
	D_ASSERT(!columns.empty());

	// We stored the table statistics as a unit in FinalizeTable.
	BinaryDeserializer stats_deserializer(reader);
	stats_deserializer.Begin();
	info.data->table_stats.Deserialize(stats_deserializer, columns);
	stats_deserializer.End();

	// Deserialize the row group pointers (lazily, just set the count and the pointer to them for now)
	info.data->row_group_count = reader.Read<uint64_t>();
	info.data->block_pointer = reader.GetMetaBlockPointer();
}

} // namespace goose
