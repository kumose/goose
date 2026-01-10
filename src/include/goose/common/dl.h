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

#include <goose/common/windows.h>
#include <goose/common/local_file_system.h>
#include <goose/common/windows_util.h>

#ifndef _WIN32
#include <dlfcn.h>
#else
#define RTLD_NOW   0
#define RTLD_LOCAL 0
#endif

namespace goose {

#ifdef _WIN32

inline void *dlopen(const char *file, int mode) {
	if (!file) {
		return (void *)GetModuleHandle(nullptr);
	}
	auto fpath = WindowsUtil::UTF8ToUnicode(file);
	return (void *)LoadLibraryW(fpath.c_str());
}

inline void *dlsym(void *handle, const char *name) {
	D_ASSERT(handle);
	return (void *)GetProcAddress((HINSTANCE)handle, name);
}

inline std::string GetDLError(void) {
	return LocalFileSystem::GetLastErrorAsString();
}

inline void dlclose(void *handle) {
	D_ASSERT(handle);
	FreeLibrary((HINSTANCE)handle);
}

#else

inline std::string GetDLError(void) {
	return dlerror();
}

#endif

} // namespace goose
