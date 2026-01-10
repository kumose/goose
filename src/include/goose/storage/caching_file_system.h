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

#include <goose/common/enums/cache_validation_mode.h>
#include <goose/common/file_open_flags.h>
#include <goose/common/open_file_info.h>
#include <goose/common/winapi.h>
#include <goose/common/shared_ptr.h>
#include <goose/main/client_context.h>
#include <goose/storage/storage_lock.h>
#include <goose/storage/external_file_cache.h>

namespace goose {

class ClientContext;
class QueryContext;
class BufferHandle;
class FileOpenFlags;
class FileSystem;
struct FileHandle;
class CachingFileSystem;

struct CachingFileHandle {
public:
	using CachedFileRangeOverlap = ExternalFileCache::CachedFileRangeOverlap;
	using CachedFileRange = ExternalFileCache::CachedFileRange;
	using CachedFile = ExternalFileCache::CachedFile;

public:
	GOOSE_API CachingFileHandle(QueryContext context, CachingFileSystem &caching_file_system, const OpenFileInfo &path,
	                             FileOpenFlags flags, CachedFile &cached_file);
	GOOSE_API ~CachingFileHandle();

public:
	//! Get the underlying FileHandle
	GOOSE_API FileHandle &GetFileHandle();
	//! Read (seek) nr_bytes from the file (or cache) at location. The pointer will be set to the requested range
	//! The buffer is guaranteed to stay in memory as long as the returned BufferHandle is in scope
	GOOSE_API BufferHandle Read(data_ptr_t &buffer, idx_t nr_bytes, idx_t location);
	//! Read (non-seeking) nr bytes from the file (or cache), same as above, also sets nr_bytes to actually read bytes
	GOOSE_API BufferHandle Read(data_ptr_t &buffer, idx_t &nr_bytes);
	//! Get some properties of the file
	GOOSE_API string GetPath() const;
	GOOSE_API idx_t GetFileSize();
	GOOSE_API timestamp_t GetLastModifiedTime();
	GOOSE_API string GetVersionTag();
	GOOSE_API bool Validate() const;
	GOOSE_API bool CanSeek();
	GOOSE_API bool IsRemoteFile() const;
	GOOSE_API bool OnDiskFile();
	GOOSE_API idx_t SeekPosition();
	GOOSE_API void Seek(idx_t location);

private:
	//! Get the version tag of the file (for checking cache invalidation)
	const string &GetVersionTag(const unique_ptr<StorageLockKey> &guard);
	//! Tries to read from the cache, filling "overlapping_ranges" with ranges that overlap with the request.
	//! Returns an invalid BufferHandle if it fails
	BufferHandle TryReadFromCache(data_ptr_t &buffer, idx_t nr_bytes, idx_t location,
	                              vector<shared_ptr<CachedFileRange>> &overlapping_ranges,
	                              optional_idx &start_location_of_next_range);
	//! Try to read from the specified range, return an invalid BufferHandle if it fails
	BufferHandle TryReadFromFileRange(const unique_ptr<StorageLockKey> &guard, CachedFileRange &file_range,
	                                  data_ptr_t &buffer, idx_t nr_bytes, idx_t location);
	//! Try to insert the file range into the cache
	BufferHandle TryInsertFileRange(BufferHandle &pin, data_ptr_t &buffer, idx_t nr_bytes, idx_t location,
	                                shared_ptr<CachedFileRange> &new_file_range);
	//! Read from file and copy from cached buffers until the requested read is complete
	//! If actually_read is false, no reading happens, only the number of non-cached reads is counted and returned
	idx_t ReadAndCopyInterleaved(const vector<shared_ptr<CachedFileRange>> &overlapping_ranges,
	                             const shared_ptr<CachedFileRange> &new_file_range, data_ptr_t buffer, idx_t nr_bytes,
	                             idx_t location, bool actually_read);

private:
	QueryContext context;

	//! The client caching file system that was used to create this CachingFileHandle
	CachingFileSystem &caching_file_system;
	//! The DB external file cache
	ExternalFileCache &external_file_cache;
	//! For opening the file (possibly with extra info)
	OpenFileInfo path;
	//! Flags used to open the file
	FileOpenFlags flags;
	//! Cache validation mode for this file
	CacheValidationMode validate;
	//! The associated CachedFile with cached ranges
	CachedFile &cached_file;

	//! The underlying FileHandle (optional)
	unique_ptr<FileHandle> file_handle;
	//! Last modified time and version tag (if FileHandle is opened)
	timestamp_t last_modified;
	string version_tag;

	//! Current position (if non-seeking reads)
	idx_t position;
};

//! CachingFileSystem is a read-only file system that closely resembles the FileSystem API.
//! Instead of reading into a designated buffer, it caches reads using the BufferManager,
//! it returns a BufferHandle and sets a pointer into it
class CachingFileSystem {
private:
	friend struct CachingFileHandle;

public:
	// Notice, the provided [file_system] should be a raw, non-caching filesystem.
	GOOSE_API CachingFileSystem(FileSystem &file_system, DatabaseInstance &db);
	GOOSE_API ~CachingFileSystem();

public:
	GOOSE_API static CachingFileSystem Get(ClientContext &context);

	GOOSE_API unique_ptr<CachingFileHandle> OpenFile(const OpenFileInfo &path, FileOpenFlags flags);
	GOOSE_API unique_ptr<CachingFileHandle> OpenFile(QueryContext context, const OpenFileInfo &path,
	                                                  FileOpenFlags flags);

private:
	//! The Client FileSystem (needs to be client-specific so we can do, e.g., HTTPFS profiling)
	FileSystem &file_system;
	//! The External File Cache that caches the files
	ExternalFileCache &external_file_cache;
	//! The DatabaseInstance.
	DatabaseInstance &db;
};

} // namespace goose
