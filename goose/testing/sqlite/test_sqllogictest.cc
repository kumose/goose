#include <goose/testing/catch.h>
#include <goose/goose.h>
#include <goose/common/string_util.h>
#include <goose/main/extension/generated_extension_loader.h>
#include <goose/parser/parser.h>
#include <goose/testing/sqlite/sqllogic_test_runner.h>
#include <goose/testing/test_helpers.h>
#include <goose/testing/test_config.h>
#include <goose/testing/scripts_runner.h>
#include <functional>
#include <string>
#include <system_error>
#include <vector>

using namespace goose;
using namespace std;

// code below traverses the test directory and makes individual test cases out
// of each script
static void listFiles(FileSystem &fs, const string &path, std::function<void(const string &)> cb) {
    fs.ListFiles(path, [&](string fname, bool is_dir) {
        string full_path = fs.JoinPath(path, fname);
        if (is_dir) {
            // recurse into directory
            listFiles(fs, full_path, cb);
        } else {
            cb(full_path);
        }
    });
}

static bool endsWith(const string &mainStr, const string &toMatch) {
    return (mainStr.size() >= toMatch.size() &&
            mainStr.compare(mainStr.size() - toMatch.size(), toMatch.size(), toMatch) == 0);
}

template<bool VERIFICATION, bool AUTO_SWITCH_TEST_DIR = false>
static void testRunner() {
    // this is an ugly hack that uses the test case name to pass the script file
    // name if someone has a better idea...
    const auto name = Catch::getResultCapture().getCurrentTestName();
    const auto test_dir_path = TestDirectoryPath(); // can vary between tests, and does IO
    auto &test_config = TestConfiguration::Get();

    string initial_dbpath = test_config.GetInitialDBPath();
    test_config.ProcessPath(initial_dbpath, name);
    if (!initial_dbpath.empty()) {
        auto test_path = StringUtil::Replace(initial_dbpath, test_dir_path, string());
        test_path = StringUtil::Replace(test_path, "\\", "/");
        auto components = StringUtil::Split(test_path, "/");
        components.pop_back();
        string total_path = test_dir_path;
        for (auto &component: components) {
            if (component.empty()) {
                continue;
            }
            total_path = TestJoinPath(total_path, component);
            TestCreateDirectory(total_path);
        }
    }
    SQLLogicTestRunner runner(std::move(initial_dbpath));
    runner.output_sql = Catch::getCurrentContext().getConfig()->outputSQL();
    runner.enable_verification = VERIFICATION;

    string prev_directory;

    // We assume the test working dir for extensions to be one dir above the test/sql. Note that this is very hacky.
    // however for now it suffices: we use it to run tests from out-of-tree extensions that are based on the extension
    // template which adheres to this convention.
    if (AUTO_SWITCH_TEST_DIR) {
        prev_directory = TestGetCurrentDirectory();

        std::size_t found = name.rfind("/test/sql");
        if (found == std::string::npos) {
            throw InvalidInputException("Failed to auto detect working dir for test '" + name +
                                        "' because a non-standard path was used!");
        }
        auto test_working_dir = name.substr(0, found);
        test_config.ChangeWorkingDirectory(test_working_dir);
    }

    // setup this test runner with Config-based env, then override with ephemerals (only WORKING_DIR at this point)
    for (auto &kv: test_config.GetTestEnvMap()) {
        runner.environment_variables[kv.first] = kv.second;
    }
    // Per runner vars
    runner.environment_variables["WORKING_DIR"] = TestGetCurrentDirectory();
    runner.environment_variables["TEST_NAME"] = name;
    runner.environment_variables["TEST_NAME__NO_SLASH"] = StringUtil::Replace(name, "/", "_");

    ErrorData error;
    try {
        runner.ExecuteFile(name);
    } catch (std::exception &ex) {
        error = ErrorData(ex);
    }

    if (AUTO_SWITCH_TEST_DIR) {
        test_config.ChangeWorkingDirectory(prev_directory);
    }

    auto on_cleanup = test_config.OnCleanupCommand();
    if (!on_cleanup.empty()) {
        // perform clean-up if any is defined
        try {
            if (!runner.con) {
                runner.Reconnect();
            }
            auto res = runner.con->Query(on_cleanup);
            if (res->HasError()) {
                res->GetErrorObject().Throw();
            }
        } catch (std::exception &ex) {
            string cleanup_failure = "Error while running clean-up routine:\n";
            ErrorData error(ex);
            cleanup_failure += error.Message();
            FAIL(cleanup_failure);
        }
    }

    // clear test directory after running tests
    ClearTestDirectory();

    if (error.HasError()) {
        FAIL(error.Message());
    }
}

static string ParseGroupFromPath(string file) {
    string extension = "";
    if (file.find(".test_slow") != std::string::npos) {
        // "slow" in the name indicates a slow test (i.e. only run as part of allunit)
        extension = "[.]";
    }
    if (file.find(".test_coverage") != std::string::npos) {
        // "coverage" in the name indicates a coverage test (i.e. only run as part of coverage)
        return "[coverage][.]";
    }
    // move backwards to the last slash
    int group_begin = -1, group_end = -1;
    for (idx_t i = file.size(); i > 0; i--) {
        if (file[i - 1] == '/' || file[i - 1] == '\\') {
            if (group_end == -1) {
                group_end = i - 1;
            } else {
                group_begin = i;
                return "[" + file.substr(group_begin, group_end - group_begin) + "]" + extension;
            }
        }
    }
    if (group_end == -1) {
        return "[" + file + "]" + extension;
    }
    return "[" + file.substr(0, group_end) + "]" + extension;
}

namespace goose {
    void register_sqllogic_tests() {
        auto enable_verification_excludes = get_verification_scripts();
        auto excludes = get_excludes_scripts();
        goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
        auto paths = get_test_directory();

        for (auto &dir: paths) {
            std::cerr <<dir << std::endl;
            listFiles(*fs, dir, [&](const string &path) {
                std::cerr <<path << std::endl;
                if (endsWith(path, ".test") || endsWith(path, ".test_slow") || endsWith(path, ".test_coverage")) {
                    // parse the name / group from the test
                    REGISTER_TEST_CASE(testRunner<false>, StringUtil::Replace(path, "\\", "/"),
                                       ParseGroupFromPath(path));
                }
            });
        }
    }
} // namespace goose
