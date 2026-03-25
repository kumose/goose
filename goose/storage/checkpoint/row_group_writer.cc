#include <goose/storage/checkpoint/table_data_writer.h>
#include <goose/catalog/catalog_entry/table_catalog_entry.h>
#include <goose/storage/table/column_checkpoint_state.h>
#include <goose/common/serializer/binary_serializer.h>

namespace goose {

RowGroupWriter::RowGroupWriter(TableCatalogEntry &table, PartialBlockManager &partial_block_manager)
    : table(table), partial_block_manager(partial_block_manager) {
	for (auto &col : table.GetColumns().Physical()) {
		compression_types.push_back(col.CompressionType());
	}
}

DatabaseInstance &RowGroupWriter::GetDatabase() {
	return table.ParentCatalog().GetDatabase();
}

SingleFileRowGroupWriter::SingleFileRowGroupWriter(TableCatalogEntry &table, PartialBlockManager &partial_block_manager,
                                                   TableDataWriter &writer, MetadataWriter &table_data_writer)
    : RowGroupWriter(table, partial_block_manager), writer(writer), table_data_writer(table_data_writer) {
}

CheckpointOptions SingleFileRowGroupWriter::GetCheckpointOptions() const {
	return writer.GetCheckpointOptions();
}

WriteStream &SingleFileRowGroupWriter::GetPayloadWriter() {
	return table_data_writer;
}

MetaBlockPointer SingleFileRowGroupWriter::GetMetaBlockPointer() {
	return table_data_writer.GetMetaBlockPointer();
}

optional_ptr<MetadataManager> SingleFileRowGroupWriter::GetMetadataManager() {
	return table_data_writer.GetManager();
}

void SingleFileRowGroupWriter::StartWritingColumns(vector<MetaBlockPointer> &column_metadata) {
	table_data_writer.SetWrittenPointers(column_metadata);
}

void SingleFileRowGroupWriter::FinishWritingColumns() {
	table_data_writer.SetWrittenPointers(nullptr);
}

} // namespace goose
