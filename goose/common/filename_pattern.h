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
#include <goose/common/types/uuid.h>

namespace goose {
    class Serializer;
    class Deserializer;

    enum class FileNameSegmentType : uint8_t { LITERAL, UUID_V4, UUID_V7, OFFSET };

    struct FileNameSegment {
        FileNameSegment() = default;

        explicit FileNameSegment(string data);

        explicit FileNameSegment(FileNameSegmentType type);

        FileNameSegmentType type;
        string data;

    public:
        void Serialize(Serializer &serializer) const;

        static FileNameSegment Deserialize(Deserializer &deserializer);
    };

    class FilenamePattern {
    public:
        FilenamePattern();

        FilenamePattern(string base, idx_t pos, bool uuid, vector<FileNameSegment> segments);

    public:
        void SetFilenamePattern(const string &pattern);

        string CreateFilename(FileSystem &fs, const string &path, const string &extension, idx_t offset) const;

        void Serialize(Serializer &serializer) const;

        static FilenamePattern Deserialize(Deserializer &deserializer);

        bool HasUUID() const;

    public:
        // serialization code for backwards compatibility
        string SerializeBase() const;

        idx_t SerializePos() const;

        vector<FileNameSegment> SerializeSegments() const;

    private:
        vector<FileNameSegment> segments;
    };
} // namespace goose
