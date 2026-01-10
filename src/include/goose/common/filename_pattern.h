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
