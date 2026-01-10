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

#include <goose/common/winapi.h>
#include <goose/common/file_system.h>
#include <goose/storage/caching_file_system.h>

namespace goose {

// Forward declaration.
class DatabaseInstance;
class ClientContext;
class QueryContext;
class CachingFileSystemWrapper;
struct CachingFileHandle;

//! Caching mode for CachingFileSystemWrapper.
//! By default only remote files will be cached, but it's also allowed to cache local for direct IO use case.
enum class CachingMode : uint8_t {
	// Cache all files.
	ALWAYS_CACHE = 0,
	// Only cache remote files, bypass cache for local files.
	CACHE_REMOTE_ONLY = 1,
};

//! CachingFileHandleWrapper wraps CachingFileHandle to conform to FileHandle API.
class CachingFileHandleWrapper : public FileHandle {
	friend class CachingFileSystemWrapper;

public:
	GOOSE_API CachingFileHandleWrapper(CachingFileSystemWrapper &file_system, unique_ptr<CachingFileHandle> handle,
	                                    FileOpenFlags flags);
	GOOSE_API ~CachingFileHandleWrapper() override;

	GOOSE_API void Close() override;

private:
	unique_ptr<CachingFileHandle> caching_handle;
};

//! [CachingFileSystemWrapper] is an adapter class, which wraps [CachingFileSystem] to conform to FileSystem API.
//! Different from [CachingFileSystem], which owns cache content and returns a [BufferHandle] to achieve zero-copy on
//! read, the wrapper class always copies requested byted into the provided address.
//!
//! NOTICE: Currently only read and seek operations are supported, write operations are disabled.
class CachingFileSystemWrapper : public FileSystem {
public:
	GOOSE_API CachingFileSystemWrapper(FileSystem &file_system, DatabaseInstance &db,
	                                    CachingMode mode = CachingMode::CACHE_REMOTE_ONLY);
	GOOSE_API ~CachingFileSystemWrapper() override;

	GOOSE_API static CachingFileSystemWrapper Get(ClientContext &context,
	                                               CachingMode mode = CachingMode::CACHE_REMOTE_ONLY);

	GOOSE_API std::string GetName() const override;

	GOOSE_API unique_ptr<FileHandle> OpenFile(const string &path, FileOpenFlags flags,
	                                           optional_ptr<FileOpener> opener = nullptr) override;
	GOOSE_API unique_ptr<FileHandle> OpenFile(const OpenFileInfo &path, FileOpenFlags flags,
	                                           optional_ptr<FileOpener> opener = nullptr);

	GOOSE_API void Read(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location) override;
	GOOSE_API void Write(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location) override;
	GOOSE_API int64_t Read(FileHandle &handle, void *buffer, int64_t nr_bytes) override;
	GOOSE_API int64_t Write(FileHandle &handle, void *buffer, int64_t nr_bytes) override;
	GOOSE_API bool Trim(FileHandle &handle, idx_t offset_bytes, idx_t length_bytes) override;

	GOOSE_API int64_t GetFileSize(FileHandle &handle) override;
	GOOSE_API timestamp_t GetLastModifiedTime(FileHandle &handle) override;
	GOOSE_API string GetVersionTag(FileHandle &handle) override;
	GOOSE_API FileType GetFileType(FileHandle &handle) override;
	GOOSE_API FileMetadata Stats(FileHandle &handle) override;
	GOOSE_API void Truncate(FileHandle &handle, int64_t new_size) override;
	GOOSE_API void FileSync(FileHandle &handle) override;

	GOOSE_API bool DirectoryExists(const string &directory, optional_ptr<FileOpener> opener = nullptr) override;
	GOOSE_API void CreateDirectory(const string &directory, optional_ptr<FileOpener> opener = nullptr) override;
	GOOSE_API void CreateDirectoriesRecursive(const string &path, optional_ptr<FileOpener> opener = nullptr) override;
	GOOSE_API void RemoveDirectory(const string &directory, optional_ptr<FileOpener> opener = nullptr) override;

	GOOSE_API bool ListFiles(const string &directory, const std::function<void(const string &, bool)> &callback,
	                          FileOpener *opener = nullptr) override;

	GOOSE_API void MoveFile(const string &source, const string &target,
	                         optional_ptr<FileOpener> opener = nullptr) override;
	GOOSE_API bool FileExists(const string &filename, optional_ptr<FileOpener> opener = nullptr) override;
	GOOSE_API bool IsPipe(const string &filename, optional_ptr<FileOpener> opener = nullptr) override;
	GOOSE_API void RemoveFile(const string &filename, optional_ptr<FileOpener> opener = nullptr) override;
	GOOSE_API bool TryRemoveFile(const string &filename, optional_ptr<FileOpener> opener = nullptr) override;

	GOOSE_API string GetHomeDirectory() override;
	GOOSE_API string ExpandPath(const string &path) override;
	GOOSE_API string PathSeparator(const string &path) override;

	GOOSE_API vector<OpenFileInfo> Glob(const string &path, FileOpener *opener = nullptr) override;

	GOOSE_API void RegisterSubSystem(unique_ptr<FileSystem> sub_fs) override;
	GOOSE_API void RegisterSubSystem(FileCompressionType compression_type, unique_ptr<FileSystem> fs) override;
	GOOSE_API void UnregisterSubSystem(const string &name) override;
	GOOSE_API unique_ptr<FileSystem> ExtractSubSystem(const string &name) override;
	GOOSE_API vector<string> ListSubSystems() override;
	GOOSE_API bool CanHandleFile(const string &fpath) override;

	GOOSE_API void Seek(FileHandle &handle, idx_t location) override;
	GOOSE_API void Reset(FileHandle &handle) override;
	GOOSE_API idx_t SeekPosition(FileHandle &handle) override;

	GOOSE_API bool IsManuallySet() override;
	GOOSE_API bool CanSeek() override;
	GOOSE_API bool OnDiskFile(FileHandle &handle) override;

	GOOSE_API unique_ptr<FileHandle> OpenCompressedFile(QueryContext context, unique_ptr<FileHandle> handle,
	                                                     bool write) override;

	GOOSE_API void SetDisabledFileSystems(const vector<string> &names) override;
	GOOSE_API bool SubSystemIsDisabled(const string &name) override;
	GOOSE_API bool IsDisabledForPath(const string &path) override;

protected:
	GOOSE_API unique_ptr<FileHandle> OpenFileExtended(const OpenFileInfo &path, FileOpenFlags flags,
	                                                   optional_ptr<FileOpener> opener) override;
	GOOSE_API bool SupportsOpenFileExtended() const override;
	GOOSE_API bool ListFilesExtended(const string &directory, const std::function<void(OpenFileInfo &info)> &callback,
	                                  optional_ptr<FileOpener> opener) override;
	GOOSE_API bool SupportsListFilesExtended() const override;

private:
	bool ShouldUseCache(const string &path) const;

	// Return an optional caching file handle, if certain filepath is cached.
	CachingFileHandle *GetCachingHandleIfPossible(FileHandle &handle);

	CachingFileSystem caching_file_system;
	FileSystem &underlying_file_system;
	CachingMode caching_mode;
};

} // namespace goose
