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
//


#include <goose/common/pipe_file_system.h>
#include <goose/common/exception.h>
#include <goose/common/file_system.h>
#include <goose/common/helper.h>
#include <goose/common/local_file_system.h>
#include <goose/common/numeric_utils.h>
#include <goose/main/client_context.h>

namespace goose {
    class PipeFile : public FileHandle {
    public:
        explicit PipeFile(QueryContext context_p, unique_ptr<FileHandle> child_handle_p)
            : FileHandle(pipe_fs, child_handle_p->path, child_handle_p->GetFlags()),
              child_handle(std::move(child_handle_p)), context(context_p) {
        }

        PipeFileSystem pipe_fs;
        unique_ptr<FileHandle> child_handle;

    public:
        int64_t ReadChunk(void *buffer, int64_t nr_bytes);

        int64_t WriteChunk(void *buffer, int64_t nr_bytes);

        void Close() override {
        }

    private:
        QueryContext context;
    };

    int64_t PipeFile::ReadChunk(void *buffer, int64_t nr_bytes) {
        return child_handle->Read(context, buffer, UnsafeNumericCast<idx_t>(nr_bytes));
    }

    int64_t PipeFile::WriteChunk(void *buffer, int64_t nr_bytes) {
        return child_handle->Write(buffer, UnsafeNumericCast<idx_t>(nr_bytes));
    }

    void PipeFileSystem::Reset(FileHandle &handle) {
        throw InternalException("Cannot reset pipe file system");
    }

    int64_t PipeFileSystem::Read(FileHandle &handle, void *buffer, int64_t nr_bytes) {
        auto &pipe = handle.Cast<PipeFile>();
        return pipe.ReadChunk(buffer, nr_bytes);
    }

    int64_t PipeFileSystem::Write(FileHandle &handle, void *buffer, int64_t nr_bytes) {
        auto &pipe = handle.Cast<PipeFile>();
        return pipe.WriteChunk(buffer, nr_bytes);
    }

    int64_t PipeFileSystem::GetFileSize(FileHandle &handle) {
        return 0;
    }

    timestamp_t PipeFileSystem::GetLastModifiedTime(FileHandle &handle) {
        auto &child_handle = *handle.Cast<PipeFile>().child_handle;
        return child_handle.file_system.GetLastModifiedTime(child_handle);
    }

    void PipeFileSystem::FileSync(FileHandle &handle) {
    }

    unique_ptr<FileHandle> PipeFileSystem::OpenPipe(QueryContext context, unique_ptr<FileHandle> handle) {
        return make_uniq<PipeFile>(context, std::move(handle));
    }
} // namespace goose
