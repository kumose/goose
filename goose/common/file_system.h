// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <goose/common/common.h>
#include <goose/common/enums/file_compression_type.h>
#include <goose/common/enums/file_glob_options.h>
#include <goose/common/exception.h>
#include <goose/common/error_data.h>
#include <goose/common/file_buffer.h>
#include <goose/common/file_open_flags.h>
#include <goose/common/open_file_info.h>
#include <goose/common/optional_idx.h>
#include <goose/common/optional_ptr.h>
#include <goose/common/string.h>
#include <goose/common/types/value.h>
#include <goose/common/types-import.h>
#include <goose/common/vector.h>

#include <functional>

#undef CreateDirectory
#undef MoveFile
#undef RemoveDirectory

namespace goose {
    class AttachedDatabase;
    class DatabaseInstance;
    class FileOpener;
    class FileSystem;
    class Logger;
    class ClientContext;
    class QueryContext;

    enum class FileType {
        //! Regular file
        FILE_TYPE_REGULAR,
        //! Directory
        FILE_TYPE_DIR,
        //! FIFO named pipe
        FILE_TYPE_FIFO,
        //! Socket
        FILE_TYPE_SOCKET,
        //! Symbolic link
        FILE_TYPE_LINK,
        //! Block device
        FILE_TYPE_BLOCKDEV,
        //! Character device
        FILE_TYPE_CHARDEV,
        //! Unknown or invalid file handle
        FILE_TYPE_INVALID,
    };

    struct FileMetadata {
        int64_t file_size = -1;
        timestamp_t last_modification_time = timestamp_t::ninfinity();
        FileType file_type = FileType::FILE_TYPE_INVALID;

        // A key-value pair of the extended file metadata, which could store any attributes.
        unordered_map<string, Value> extended_file_info;
    };

    struct FileHandle {
    public:
        GOOSE_API FileHandle(FileSystem &file_system, string path, FileOpenFlags flags);

        FileHandle(const FileHandle &) = delete;

        GOOSE_API virtual ~FileHandle();

        // Read at [nr_bytes] bytes into [buffer], and return the bytes actually read.
        // File offset will be changed, which advances for number of bytes read.
        GOOSE_API int64_t Read(void *buffer, idx_t nr_bytes);

        GOOSE_API int64_t Read(QueryContext context, void *buffer, idx_t nr_bytes);

        GOOSE_API int64_t Write(void *buffer, idx_t nr_bytes);

        // Read at [nr_bytes] bytes into [buffer].
        // File offset will not be changed.
        GOOSE_API void Read(void *buffer, idx_t nr_bytes, idx_t location);

        GOOSE_API void Read(QueryContext context, void *buffer, idx_t nr_bytes, idx_t location);

        GOOSE_API void Write(QueryContext context, void *buffer, idx_t nr_bytes, idx_t location);

        GOOSE_API void Seek(idx_t location);

        GOOSE_API void Reset();

        GOOSE_API idx_t SeekPosition();

        GOOSE_API void Sync();

        GOOSE_API void Truncate(int64_t new_size);

        GOOSE_API string ReadLine();

        GOOSE_API string ReadLine(QueryContext context);

        GOOSE_API bool Trim(idx_t offset_bytes, idx_t length_bytes);

        GOOSE_API virtual idx_t GetProgress();

        GOOSE_API virtual FileCompressionType GetFileCompressionType();

        GOOSE_API bool CanSeek();

        GOOSE_API bool IsPipe();

        GOOSE_API bool OnDiskFile();

        GOOSE_API idx_t GetFileSize();

        GOOSE_API FileType GetType();

        GOOSE_API FileMetadata Stats();

        GOOSE_API void TryAddLogger(FileOpener &opener);

        //! Closes the file handle.
        GOOSE_API virtual void Close() = 0;

        string GetPath() const {
            return path;
        }

        FileOpenFlags GetFlags() const {
            return flags;
        }

        template<class TARGET>
        TARGET &Cast() {
            DynamicCastCheck<TARGET>(this);
            return reinterpret_cast<TARGET &>(*this);
        }

        template<class TARGET>
        const TARGET &Cast() const {
            DynamicCastCheck<TARGET>(this);
            return reinterpret_cast<const TARGET &>(*this);
        }

    public:
        FileSystem &file_system;
        string path;
        FileOpenFlags flags;

        shared_ptr<Logger> logger;
    };

    class FileSystem {
    public:
        GOOSE_API virtual ~FileSystem();

    public:
        GOOSE_API static FileSystem &GetFileSystem(ClientContext &context);

        GOOSE_API static FileSystem &GetFileSystem(DatabaseInstance &db);

        GOOSE_API static FileSystem &Get(AttachedDatabase &db);

        GOOSE_API virtual unique_ptr<FileHandle> OpenFile(const string &path, FileOpenFlags flags,
                                                          optional_ptr<FileOpener> opener = nullptr);

        GOOSE_API unique_ptr<FileHandle> OpenFile(const OpenFileInfo &path, FileOpenFlags flags,
                                                  optional_ptr<FileOpener> opener = nullptr);

        //! Read exactly nr_bytes from the specified location in the file. Fails if nr_bytes could not be read. This is
        //! equivalent to calling SetFilePointer(location) followed by calling Read().
        GOOSE_API virtual void Read(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location);

        //! Write exactly nr_bytes to the specified location in the file. Fails if nr_bytes could not be written. This is
        //! equivalent to calling SetFilePointer(location) followed by calling Write().
        GOOSE_API virtual void Write(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location);

        //! Read nr_bytes from the specified file into the buffer, moving the file pointer forward by nr_bytes. Returns the
        //! amount of bytes read.
        GOOSE_API virtual int64_t Read(FileHandle &handle, void *buffer, int64_t nr_bytes);

        //! Write nr_bytes from the buffer into the file, moving the file pointer forward by nr_bytes.
        GOOSE_API virtual int64_t Write(FileHandle &handle, void *buffer, int64_t nr_bytes);

        //! Excise a range of the file. The OS can drop pages from the page-cache, and the file-system is free to deallocate
        //! this range (sparse file support). Reads to the range will succeed but will return undefined data.
        GOOSE_API virtual bool Trim(FileHandle &handle, idx_t offset_bytes, idx_t length_bytes);

        //! Returns the file size of a file handle, returns -1 on error
        GOOSE_API virtual int64_t GetFileSize(FileHandle &handle);

        //! Returns the file last modified time of a file handle, returns timespec with zero on all attributes on error
        GOOSE_API virtual timestamp_t GetLastModifiedTime(FileHandle &handle);

        //! Returns a tag that uniquely identifies the version of the file,
        //! used for checking cache invalidation for CachingFileSystem httpfs files
        GOOSE_API virtual string GetVersionTag(FileHandle &handle);

        //! Returns the file type of the attached handle
        GOOSE_API virtual FileType GetFileType(FileHandle &handle);

        //! Returns the file stats of the attached handle.
        GOOSE_API virtual FileMetadata Stats(FileHandle &handle);

        //! Truncate a file to a maximum size of new_size, new_size should be smaller than or equal to the current size of
        //! the file
        GOOSE_API virtual void Truncate(FileHandle &handle, int64_t new_size);

        //! Check if a directory exists
        GOOSE_API virtual bool DirectoryExists(const string &directory, optional_ptr<FileOpener> opener = nullptr);

        //! Create a directory if it does not exist
        GOOSE_API virtual void CreateDirectory(const string &directory, optional_ptr<FileOpener> opener = nullptr);

        //! Helper function that uses DirectoryExists and CreateDirectory to ensure all directories in path are created
        GOOSE_API virtual void
        CreateDirectoriesRecursive(const string &path, optional_ptr<FileOpener> opener = nullptr);

        //! Recursively remove a directory and all files in it
        GOOSE_API virtual void RemoveDirectory(const string &directory, optional_ptr<FileOpener> opener = nullptr);

        //! List files in a directory, invoking the callback method for each one with (filename, is_dir)
        GOOSE_API virtual bool ListFiles(const string &directory,
                                         const std::function<void(const string &, bool)> &callback,
                                         FileOpener *opener = nullptr);

        GOOSE_API bool ListFiles(const string &directory, const std::function<void(OpenFileInfo &info)> &callback,
                                 optional_ptr<FileOpener> opener = nullptr);

        //! Move a file from source path to the target, StorageManager relies on this being an atomic action for ACID
        //! properties
        GOOSE_API virtual void MoveFile(const string &source, const string &target,
                                        optional_ptr<FileOpener> opener = nullptr);

        //! Check if a file exists
        GOOSE_API virtual bool FileExists(const string &filename, optional_ptr<FileOpener> opener = nullptr);

        //! Check if path is pipe
        GOOSE_API virtual bool IsPipe(const string &filename, optional_ptr<FileOpener> opener = nullptr);

        //! Remove a file from disk
        GOOSE_API virtual void RemoveFile(const string &filename, optional_ptr<FileOpener> opener = nullptr);

        //! Remvoe a file from disk if it exists - if it does not exist, return false
        GOOSE_API virtual bool TryRemoveFile(const string &filename, optional_ptr<FileOpener> opener = nullptr);

        //! Sync a file handle to disk
        GOOSE_API virtual void FileSync(FileHandle &handle);

        //! Sets the working directory
        GOOSE_API static void SetWorkingDirectory(const string &path);

        //! Gets the working directory
        GOOSE_API static string GetWorkingDirectory();

        //! Gets the users home directory
        GOOSE_API static string GetHomeDirectory(optional_ptr<FileOpener> opener);

        //! Gets the users home directory
        GOOSE_API virtual string GetHomeDirectory();

        //! Expands a given path, including e.g. expanding the home directory of the user
        GOOSE_API static string ExpandPath(const string &path, optional_ptr<FileOpener> opener);

        //! Expands a given path, including e.g. expanding the home directory of the user
        GOOSE_API virtual string ExpandPath(const string &path);

        //! Returns the system-available memory in bytes. Returns DConstants::INVALID_INDEX if the system function fails.
        GOOSE_API static optional_idx GetAvailableMemory();

        //! Returns the space available on the disk. Returns DConstants::INVALID_INDEX if the information was not available.
        GOOSE_API static optional_idx GetAvailableDiskSpace(const string &path);

        //! Path separator for path
        GOOSE_API virtual string PathSeparator(const string &path);

        //! Checks if path is starts with separator (i.e., '/' on UNIX '\\' on Windows)
        GOOSE_API bool IsPathAbsolute(const string &path);

        //! Normalize an absolute path - the goal of normalizing is converting "\test.db" and "C:/test.db" into "C:\test.db"
        //! so that the database system cache can correctly
        GOOSE_API string NormalizeAbsolutePath(const string &path);

        //! Join two paths together
        GOOSE_API string JoinPath(const string &a, const string &path);

        //! Convert separators in a path to the local separators (e.g. convert "/" into \\ on windows)
        GOOSE_API string ConvertSeparators(const string &path);

        //! Extract the base name of a file (e.g. if the input is lib/example.dll the base name is 'example')
        GOOSE_API string ExtractBaseName(const string &path);

        //! Extract the extension of a file (e.g. if the input is lib/example.dll the extension is 'dll')
        GOOSE_API string ExtractExtension(const string &path);

        //! Extract the name of a file (e.g if the input is lib/example.dll the name is 'example.dll')
        GOOSE_API string ExtractName(const string &path);

        //! Returns the value of an environment variable - or the empty string if it is not set
        GOOSE_API static string GetEnvVariable(const string &name);

        //! Whether there is a glob in the string
        GOOSE_API static bool HasGlob(const string &str);

        //! Runs a glob on the file system, returning a list of matching files
        GOOSE_API virtual vector<OpenFileInfo> Glob(const string &path, FileOpener *opener = nullptr);

        GOOSE_API vector<OpenFileInfo> GlobFiles(const string &path, ClientContext &context,
                                                 const FileGlobInput &input = FileGlobOptions::DISALLOW_EMPTY);

        //! registers a sub-file system to handle certain file name prefixes, e.g. http:// etc.
        GOOSE_API virtual void RegisterSubSystem(unique_ptr<FileSystem> sub_fs);

        GOOSE_API virtual void RegisterSubSystem(FileCompressionType compression_type, unique_ptr<FileSystem> fs);

        //! Unregister a sub-filesystem by name
        GOOSE_API virtual void UnregisterSubSystem(const string &name);

        // !Extract a sub-filesystem by name, with ownership transfered, return nullptr if not registered or the subsystem
        // has been disabled.
        GOOSE_API virtual unique_ptr<FileSystem> ExtractSubSystem(const string &name);

        //! List registered sub-filesystems, including builtin ones
        GOOSE_API virtual vector<string> ListSubSystems();

        //! Whether or not a sub-system can handle a specific file path
        GOOSE_API virtual bool CanHandleFile(const string &fpath);

        //! Set the file pointer of a file handle to a specified location. Reads and writes will happen from this location
        GOOSE_API virtual void Seek(FileHandle &handle, idx_t location);

        //! Reset a file to the beginning (equivalent to Seek(handle, 0) for simple files)
        GOOSE_API virtual void Reset(FileHandle &handle);

        GOOSE_API virtual idx_t SeekPosition(FileHandle &handle);

        //! If FS was manually set by the user
        GOOSE_API virtual bool IsManuallySet();

        //! Whether or not we can seek into the file
        GOOSE_API virtual bool CanSeek();

        //! Whether or not the FS handles plain files on disk. This is relevant for certain optimizations, as random reads
        //! in a file on-disk are much cheaper than e.g. random reads in a file over the network
        GOOSE_API virtual bool OnDiskFile(FileHandle &handle);

        GOOSE_API virtual unique_ptr<FileHandle> OpenCompressedFile(QueryContext context, unique_ptr<FileHandle> handle,
                                                                    bool write);

        //! Create a LocalFileSystem.
        GOOSE_API static unique_ptr<FileSystem> CreateLocal();

        //! Return the name of the filesytem. Used for forming diagnosis messages.
        GOOSE_API virtual std::string GetName() const = 0;

        //! Whether or not a file is remote or local, based only on file path
        GOOSE_API static bool IsRemoteFile(const string &path);

        GOOSE_API static bool IsRemoteFile(const string &path, string &extension);

        GOOSE_API virtual void SetDisabledFileSystems(const vector<string> &names);

        GOOSE_API virtual bool SubSystemIsDisabled(const string &name);

        //! Check if the filesystem that would handle this path is disabled
        GOOSE_API virtual bool IsDisabledForPath(const string &path);

        GOOSE_API static bool IsDirectory(const OpenFileInfo &info);

    protected:
        GOOSE_API virtual unique_ptr<FileHandle> OpenFileExtended(const OpenFileInfo &path, FileOpenFlags flags,
                                                                  optional_ptr<FileOpener> opener);

        GOOSE_API virtual bool SupportsOpenFileExtended() const;

        GOOSE_API virtual bool ListFilesExtended(const string &directory,
                                                 const std::function<void(OpenFileInfo &info)> &callback,
                                                 optional_ptr<FileOpener> opener);

        GOOSE_API virtual bool SupportsListFilesExtended() const;

    public:
        template<class TARGET>
        TARGET &Cast() {
            DynamicCastCheck<TARGET>(this);
            return reinterpret_cast<TARGET &>(*this);
        }

        template<class TARGET>
        const TARGET &Cast() const {
            DynamicCastCheck<TARGET>(this);
            return reinterpret_cast<const TARGET &>(*this);
        }
    };
} // namespace goose
