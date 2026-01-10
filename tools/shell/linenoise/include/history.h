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

#include <goose/common/common.h>

namespace goose {

class History {
public:
	static void Free();
	static idx_t GetLength();
	static const char *GetEntry(idx_t index);
	static void Overwrite(idx_t index, const char *new_entry);
	static void RemoveLastEntry();
	static int Add(const char *line);
	static int Add(const char *line, idx_t len);
	static int SetMaxLength(idx_t len);
	static int Save(const char *filename);
	static int Load(const char *filename);
};

} // namespace goose
