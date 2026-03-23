#include <goose/storage/magic_bytes.h>
#include <goose/main/client_context.h>
#include <goose/common/local_file_system.h>
#include <goose/storage/storage_info.h>

namespace goose {

DataFileType MagicBytes::CheckMagicBytes(QueryContext context, FileSystem &fs, const string &path) {
	if (path.empty() || path == IN_MEMORY_PATH) {
		return DataFileType::GOOSE_FILE;
	}
	auto handle = fs.OpenFile(path, FileFlags::FILE_FLAGS_READ | FileFlags::FILE_FLAGS_NULL_IF_NOT_EXISTS);
	if (!handle) {
		return DataFileType::FILE_DOES_NOT_EXIST;
	}

	constexpr const idx_t MAGIC_BYTES_READ_SIZE = 16;
	char buffer[MAGIC_BYTES_READ_SIZE] = {};

	handle->Read(context, buffer, MAGIC_BYTES_READ_SIZE);
	if (memcmp(buffer, "SQLite format 3\0", 16) == 0) {
		return DataFileType::SQLITE_FILE;
	}
	if (memcmp(buffer, "PAR1", 4) == 0) {
		return DataFileType::PARQUET_FILE;
	}
	if (memcmp(buffer + MainHeader::MAGIC_BYTE_OFFSET, MainHeader::MAGIC_BYTES, MainHeader::MAGIC_BYTE_SIZE) == 0) {
		return DataFileType::GOOSE_FILE;
	}
	return DataFileType::UNKNOWN_FILE;
}

} // namespace goose
