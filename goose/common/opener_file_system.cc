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


#include <goose/common/opener_file_system.h>
#include <goose/common/file_opener.h>
#include <goose/main/database.h>
#include <goose/main/config.h>

namespace goose {
    void OpenerFileSystem::VerifyNoOpener(optional_ptr<FileOpener> opener) {
        if (opener) {
            throw InternalException("OpenerFileSystem cannot take an opener - the opener is pushed automatically");
        }
    }

    void OpenerFileSystem::VerifyCanAccessFileInternal(const string &path, FileType type) {
        auto opener = GetOpener();
        if (!opener) {
            return;
        }
        auto db = opener->TryGetDatabase();
        if (!db) {
            return;
        }
        auto &config = db->config;
        if (!config.CanAccessFile(path, type)) {
            throw PermissionException("Cannot access %s \"%s\" - file system operations are disabled by configuration",
                                      type == FileType::FILE_TYPE_DIR ? "directory" : "file", path);
        }
    }

    void OpenerFileSystem::VerifyCanAccessFile(const string &path) {
        VerifyCanAccessFileInternal(path, FileType::FILE_TYPE_REGULAR);
    }

    void OpenerFileSystem::VerifyCanAccessDirectory(const string &path) {
        VerifyCanAccessFileInternal(path, FileType::FILE_TYPE_DIR);
    }
} // namespace goose
