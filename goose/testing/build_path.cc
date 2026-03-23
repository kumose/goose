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

#include <goose/testing/build_path.h>

namespace goose {

    static std::string goose_build_directory;
    void set_goose_build_directory(const std::string &path) {
        goose_build_directory = path;
    }
#ifndef GOOSE_BUILD_DIRECTORY
    std::string get_goose_build_directory() {
        return goose_build_directory;
    }
#else
    std::string get_goose_build_directory() {
        return GOOSE_BUILD_DIRECTORY;
    }
#endif
}  // namespace goose
