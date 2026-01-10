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
#include <goose/common/constants.h>
#include <goose/common/exception.h>
#include <goose/common/vector.h>
#include <goose/common/windows.h>

namespace goose {

#ifdef GOOSE_WINDOWS
class WindowsUtil {
public:
	//! Windows helper functions
	static std::wstring UTF8ToUnicode(const char *input);
	static string UnicodeToUTF8(LPCWSTR input);
	static string UTF8ToMBCS(const char *input, bool use_ansi = false);
};
#endif

} // namespace goose
