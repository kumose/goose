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

#ifndef GOOSE_API
#if defined(_WIN32) && !defined(__MINGW32__)
#ifdef GOOSE_STATIC_BUILD
#define GOOSE_API
#else
#if defined(GOOSE_BUILD_LIBRARY) && !defined(GOOSE_BUILD_LOADABLE_EXTENSION)
#define GOOSE_API __declspec(dllexport)
#else
#define GOOSE_API __declspec(dllimport)
#endif
#endif
#else
#define GOOSE_API
#endif
#endif

#ifndef GOOSE_EXTENSION_API
#ifdef _WIN32
#ifdef GOOSE_STATIC_BUILD
#define GOOSE_EXTENSION_API
#else
#define GOOSE_EXTENSION_API __declspec(dllexport)
#endif
#else
#define GOOSE_EXTENSION_API __attribute__((visibility("default")))
#endif
#endif
