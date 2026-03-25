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

#include <vector>
#include <string>
#include <goose/testing/scripts_runner.h>

/*
 void RegisterSqllogictests() {
        vector<string> enable_verification_excludes = {
            // too slow for verification
            "test/select5.test",
            "test/index",
            // optimization masks int32 overflow
            "test/random/aggregates/slt_good_102.test",
            "test/random/aggregates/slt_good_11.test",
            "test/random/aggregates/slt_good_115.test",
            "test/random/aggregates/slt_good_116.test",
            "test/random/aggregates/slt_good_118.test",
            "test/random/aggregates/slt_good_119.test",
            "test/random/aggregates/slt_good_122.test",
            "test/random/aggregates/slt_good_17.test",
            "test/random/aggregates/slt_good_20.test",
            "test/random/aggregates/slt_good_23.test",
            "test/random/aggregates/slt_good_25.test",
            "test/random/aggregates/slt_good_3.test",
            "test/random/aggregates/slt_good_30.test",
            "test/random/aggregates/slt_good_31.test",
            "test/random/aggregates/slt_good_38.test",
            "test/random/aggregates/slt_good_39.test",
            "test/random/aggregates/slt_good_4.test",
            "test/random/aggregates/slt_good_43.test",
            "test/random/aggregates/slt_good_46.test",
            "test/random/aggregates/slt_good_51.test",
            "test/random/aggregates/slt_good_56.test",
            "test/random/aggregates/slt_good_66.test",
            "test/random/aggregates/slt_good_7.test",
            "test/random/aggregates/slt_good_72.test",
            "test/random/aggregates/slt_good_82.test",
            "test/random/aggregates/slt_good_84.test",
            "test/random/aggregates/slt_good_85.test",
            "test/random/aggregates/slt_good_91.test",
            "test/random/expr/slt_good_15.test",
            "test/random/expr/slt_good_66.test",
            "test/random/expr/slt_good_91.test",
        };
        vector<string> excludes = {
            // tested separately
            "test/select1.test", "test/select2.test", "test/select3.test", "test/select4.test",
            // feature not supported
            "evidence/slt_lang_replace.test", // INSERT OR REPLACE
            "evidence/slt_lang_reindex.test", // REINDEX
            "evidence/slt_lang_update.test", // Multiple assignments to same column "x" in update
            "evidence/slt_lang_createtrigger.test", // TRIGGER
            "evidence/slt_lang_droptrigger.test", // TRIGGER
            // no + for varchar columns
            "test/index/random/10/slt_good_14.test", "test/index/random/10/slt_good_1.test",
            "test/index/random/10/slt_good_0.test", "test/index/random/10/slt_good_12.test",
            "test/index/random/10/slt_good_6.test", "test/index/random/10/slt_good_13.test",
            "test/index/random/10/slt_good_5.test", "test/index/random/10/slt_good_10.test",
            "test/index/random/10/slt_good_11.test", "test/index/random/10/slt_good_4.test",
            "test/index/random/10/slt_good_8.test", "test/index/random/10/slt_good_3.test",
            "test/index/random/10/slt_good_2.test", "test/index/random/100/slt_good_1.test",
            "test/index/random/100/slt_good_0.test", "test/index/random/1000/slt_good_0.test",
            "test/index/random/1000/slt_good_7.test", "test/index/random/1000/slt_good_6.test",
            "test/index/random/1000/slt_good_5.test", "test/index/random/1000/slt_good_8.test",
            // overflow in 32-bit integer multiplication (sqlite does automatic upcasting)
            "test/random/aggregates/slt_good_96.test", "test/random/aggregates/slt_good_75.test",
            "test/random/aggregates/slt_good_64.test", "test/random/aggregates/slt_good_9.test",
            "test/random/aggregates/slt_good_110.test", "test/random/aggregates/slt_good_101.test",
            "test/random/expr/slt_good_55.test", "test/random/expr/slt_good_115.test",
            "test/random/expr/slt_good_103.test",
            "test/random/expr/slt_good_80.test", "test/random/expr/slt_good_75.test",
            "test/random/expr/slt_good_42.test",
            "test/random/expr/slt_good_49.test", "test/random/expr/slt_good_24.test",
            "test/random/expr/slt_good_30.test",
            "test/random/expr/slt_good_8.test", "test/random/expr/slt_good_61.test",
            // dependencies between tables/views prevent dropping in Goose without CASCADE
            "test/index/view/1000/slt_good_0.test", "test/index/view/100/slt_good_0.test",
            "test/index/view/100/slt_good_5.test", "test/index/view/100/slt_good_1.test",
            "test/index/view/100/slt_good_3.test", "test/index/view/100/slt_good_4.test",
            "test/index/view/100/slt_good_2.test", "test/index/view/10000/slt_good_0.test",
            "test/index/view/10/slt_good_5.test", "test/index/view/10/slt_good_7.test",
            "test/index/view/10/slt_good_1.test", "test/index/view/10/slt_good_3.test",
            "test/index/view/10/slt_good_4.test", "test/index/view/10/slt_good_6.test",
            "test/index/view/10/slt_good_2.test",
            // strange error in hash comparison, results appear correct...
            "test/index/random/10/slt_good_7.test", "test/index/random/10/slt_good_9.test"
        };
        goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
        listFiles(*fs, fs->JoinPath(fs->JoinPath("third_party", "sqllogictest"), "test"), [&](const string &path) {
            if (endsWith(path, ".test")) {
                for (auto &excl: excludes) {
                    if (path.find(excl) != string::npos) {
                        return;
                    }
                }
                bool enable_verification = true;
                for (auto &excl: enable_verification_excludes) {
                    if (path.find(excl) != string::npos) {
                        enable_verification = false;
                        break;
                    }
                }
                if (enable_verification) {
                    REGISTER_TEST_CASE(testRunner<true>, StringUtil::Replace(path, "\\", "/"), "[sqlitelogic][.]");
                } else {
                    REGISTER_TEST_CASE(testRunner<false>, StringUtil::Replace(path, "\\", "/"), "[sqlitelogic][.]");
                }
            }
        });
        listFiles(*fs, "test", [&](const string &path) {
            if (endsWith(path, ".test") || endsWith(path, ".test_slow") || endsWith(path, ".test_coverage")) {
                // parse the name / group from the test
                REGISTER_TEST_CASE(testRunner<false>, StringUtil::Replace(path, "\\", "/"), ParseGroupFromPath(path));
            }
        });

        for (const auto &extension_test_path: ExtensionHelper::LoadedExtensionTestPaths()) {
            listFiles(*fs, extension_test_path, [&](const string &path) {
                if (endsWith(path, ".test") || endsWith(path, ".test_slow") || endsWith(path, ".test_coverage")) {
                    auto fun = testRunner<false, true>;
                    REGISTER_TEST_CASE(fun, StringUtil::Replace(path, "\\", "/"), ParseGroupFromPath(path));
                }
            });
        }
    }
*/