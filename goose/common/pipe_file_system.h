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
    class PipeFileSystem : public FileSystem {
    public:
        static unique_ptr<FileHandle> OpenPipe(QueryContext context, unique_ptr<FileHandle> handle);

        int64_t Read(FileHandle &handle, void *buffer, int64_t nr_bytes) override;

        int64_t Write(FileHandle &handle, void *buffer, int64_t nr_bytes) override;

        int64_t GetFileSize(FileHandle &handle) override;

        timestamp_t GetLastModifiedTime(FileHandle &handle) override;

        void Reset(FileHandle &handle) override;

        bool OnDiskFile(FileHandle &handle) override {
            return false;
        };

        bool CanSeek() override {
            return false;
        }

        bool IsPipe(const string &filename, optional_ptr<FileOpener> opener) override {
            return true;
        }

        void FileSync(FileHandle &handle) override;

        std::string GetName() const override {
            return "PipeFileSystem";
        }
    };
} // namespace goose
