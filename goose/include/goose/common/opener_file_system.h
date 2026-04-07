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

#include <goose/common/file_system.h>

namespace goose {
    // The OpenerFileSystem is wrapper for a file system that pushes an appropriate FileOpener into the various API calls
    class OpenerFileSystem : public FileSystem {
    public:
        virtual FileSystem &GetFileSystem() const = 0;

        virtual optional_ptr<FileOpener> GetOpener() const = 0;

        void VerifyNoOpener(optional_ptr<FileOpener> opener);

        void VerifyCanAccessDirectory(const string &path);

        void VerifyCanAccessFile(const string &path);

        void Read(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location) override {
            GetFileSystem().Read(handle, buffer, nr_bytes, location);
        }

        void Write(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location) override {
            throw InternalException("writing on the OpenerFileSystem is undefined");
        }

        int64_t Read(FileHandle &handle, void *buffer, int64_t nr_bytes) override {
            return GetFileSystem().Read(handle, buffer, nr_bytes);
        }

        int64_t Write(FileHandle &handle, void *buffer, int64_t nr_bytes) override {
            return GetFileSystem().Write(handle, buffer, nr_bytes);
        }

        int64_t GetFileSize(FileHandle &handle) override {
            return GetFileSystem().GetFileSize(handle);
        }

        timestamp_t GetLastModifiedTime(FileHandle &handle) override {
            return GetFileSystem().GetLastModifiedTime(handle);
        }

        string GetVersionTag(FileHandle &handle) override {
            return GetFileSystem().GetVersionTag(handle);
        }

        FileType GetFileType(FileHandle &handle) override {
            return GetFileSystem().GetFileType(handle);
        }

        FileMetadata Stats(FileHandle &handle) override {
            return GetFileSystem().Stats(handle);
        }

        void Truncate(FileHandle &handle, int64_t new_size) override {
            GetFileSystem().Truncate(handle, new_size);
        }

        void FileSync(FileHandle &handle) override {
            GetFileSystem().FileSync(handle);
        }

        bool DirectoryExists(const string &directory, optional_ptr<FileOpener> opener) override {
            VerifyNoOpener(opener);
            VerifyCanAccessDirectory(directory);
            return GetFileSystem().DirectoryExists(directory, GetOpener());
        }

        void CreateDirectory(const string &directory, optional_ptr<FileOpener> opener) override {
            VerifyNoOpener(opener);
            VerifyCanAccessDirectory(directory);
            return GetFileSystem().CreateDirectory(directory, GetOpener());
        }

        void RemoveDirectory(const string &directory, optional_ptr<FileOpener> opener) override {
            VerifyNoOpener(opener);
            VerifyCanAccessDirectory(directory);
            return GetFileSystem().RemoveDirectory(directory, GetOpener());
        }

        void MoveFile(const string &source, const string &target, optional_ptr<FileOpener> opener) override {
            VerifyNoOpener(opener);
            VerifyCanAccessFile(source);
            VerifyCanAccessFile(target);
            GetFileSystem().MoveFile(source, target, GetOpener());
        }

        string GetHomeDirectory() override {
            return FileSystem::GetHomeDirectory(GetOpener());
        }

        string ExpandPath(const string &path) override {
            return FileSystem::ExpandPath(path, GetOpener());
        }

        bool FileExists(const string &filename, optional_ptr<FileOpener> opener) override {
            VerifyNoOpener(opener);
            VerifyCanAccessFile(filename);
            return GetFileSystem().FileExists(filename, GetOpener());
        }

        bool IsPipe(const string &filename, optional_ptr<FileOpener> opener) override {
            VerifyNoOpener(opener);
            return GetFileSystem().IsPipe(filename, GetOpener());
        }

        void RemoveFile(const string &filename, optional_ptr<FileOpener> opener) override {
            VerifyNoOpener(opener);
            VerifyCanAccessFile(filename);
            GetFileSystem().RemoveFile(filename, GetOpener());
        }

        bool TryRemoveFile(const string &filename, optional_ptr<FileOpener> opener) override {
            VerifyNoOpener(opener);
            VerifyCanAccessFile(filename);
            return GetFileSystem().TryRemoveFile(filename, GetOpener());
        }

        string PathSeparator(const string &path) override {
            return GetFileSystem().PathSeparator(path);
        }

        vector<OpenFileInfo> Glob(const string &path, FileOpener *opener = nullptr) override {
            VerifyNoOpener(opener);
            VerifyCanAccessFile(path);
            return GetFileSystem().Glob(path, GetOpener().get());
        }

        std::string GetName() const override {
            return "OpenerFileSystem - " + GetFileSystem().GetName();
        }

        void RegisterSubSystem(unique_ptr<FileSystem> sub_fs) override {
            GetFileSystem().RegisterSubSystem(std::move(sub_fs));
        }

        void RegisterSubSystem(FileCompressionType compression_type, unique_ptr<FileSystem> fs) override {
            GetFileSystem().RegisterSubSystem(compression_type, std::move(fs));
        }

        void UnregisterSubSystem(const string &name) override {
            GetFileSystem().UnregisterSubSystem(name);
        }

        void SetDisabledFileSystems(const vector<string> &names) override {
            GetFileSystem().SetDisabledFileSystems(names);
        }

        bool SubSystemIsDisabled(const string &name) override {
            return GetFileSystem().SubSystemIsDisabled(name);
        }

        bool IsDisabledForPath(const string &path) override {
            return GetFileSystem().IsDisabledForPath(path);
        }

        vector<string> ListSubSystems() override {
            return GetFileSystem().ListSubSystems();
        }

    protected:
        unique_ptr<FileHandle> OpenFileExtended(const OpenFileInfo &file, FileOpenFlags flags,
                                                optional_ptr<FileOpener> opener = nullptr) override {
            VerifyNoOpener(opener);
            VerifyCanAccessFile(file.path);
            return GetFileSystem().OpenFile(file, flags, GetOpener());
        }

        bool SupportsOpenFileExtended() const override {
            return true;
        }

        bool ListFilesExtended(const string &directory, const std::function<void(OpenFileInfo &info)> &callback,
                               optional_ptr<FileOpener> opener) override {
            VerifyNoOpener(opener);
            VerifyCanAccessDirectory(directory);
            return GetFileSystem().ListFiles(directory, callback, GetOpener().get());
        }

        bool SupportsListFilesExtended() const override {
            return true;
        }

    private:
        void VerifyCanAccessFileInternal(const string &path, FileType type);
    };
} // namespace goose
