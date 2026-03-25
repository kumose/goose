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


#include  <goose/testing/scripts_runner.h>

namespace goose {
    std::vector<std::string> g_verification;
    std::vector<std::string> g_excludes;

    std::vector<std::string> g_dirs;

    void set_verification_scripts(const std::vector<std::string> &scripts) {
        g_verification = scripts;
    }

    void set_excludes_scripts(const std::vector<std::string> &scripts) {
        g_excludes = scripts;
    }

    const std::vector<std::string> &get_verification_scripts() {
        return g_verification;
    }

    const std::vector<std::string> &get_excludes_scripts() {
        return g_excludes;
    }

    void set_test_directory(const std::vector<std::string>& test_directory) {
        g_dirs = test_directory;
    }
    const std::vector<std::string>& get_test_directory() {
        return g_dirs;
    }
} // namespace goose
