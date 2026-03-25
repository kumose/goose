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
//

#pragma once

#define CATCH_CONFIG_RUNNER
#include <goose/testing/catch.h>
#include <goose/common/file_system.h>
#include <goose/common/string_util.h>
#include <goose/testing/sqlite/sqllogic_test_logger.h>
#include <goose/testing/test_helpers.h>
#include <goose/testing/test_config.h>
#include <goose/testing/scripts_runner.h>

namespace goose {

	struct TestingDir {
		std::string test_directory;
		static TestingDir &instance() {
			static TestingDir testing_dir;
			return testing_dir;
		}
	};

	void init_goose_testing();
}  // namespace goose

using namespace goose;

int main(int argc_in, char *argv[]) {
	init_goose_testing();
	goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
	string test_directory = TestingDir::instance().test_directory;

	auto &test_config = TestConfiguration::Get();
	test_config.Initialize();

	idx_t argc = NumericCast<idx_t>(argc_in);
	int new_argc = 0;
	auto new_argv = goose::unique_ptr<char *[]>(new char *[argc]);
	for (idx_t i = 0; i < argc; i++) {
		string argument(argv[i]);
		if (argument == "--test-dir") {
			test_directory = string(argv[++i]);
		} else if (argument == "--test-temp-dir") {
			SetDeleteTestPath(false);
			auto test_dir = string(argv[++i]);
			if (fs->DirectoryExists(test_dir)) {
				fprintf(stderr, "--test-temp-dir cannot point to a directory that already exists (%s)\n",
				        test_dir.c_str());
				return 1;
			}
			SetTestDirectory(test_dir);
		} else if (argument == "--require") {
			AddRequire(string(argv[++i]));
		} else if (!test_config.ParseArgument(argument, argc, argv, i)) {
			new_argv[new_argc] = argv[i];
			new_argc++;
		}
	}
	test_config.ChangeWorkingDirectory(test_directory);

	// delete the testing directory if it exists
	auto dir = TestCreatePath("");
	try {
		TestDeleteDirectory(dir);
		// create the empty testing directory
		TestCreateDirectory(dir);
	} catch (std::exception &ex) {
		fprintf(stderr, "Failed to create testing directory \"%s\": %s\n", dir.c_str(), ex.what());
		return 1;
	}

	if (test_config.GetSkipCompiledTests()) {
		Catch::getMutableRegistryHub().clearTests();
	}
	register_sqllogic_tests();
	int result = Catch::Session().run(new_argc, new_argv.get());

	std::string failures_summary = FailureSummary::GetFailureSummary();
	if (!failures_summary.empty()) {
		auto description = test_config.GetDescription();
		if (!description.empty()) {
			std::cerr << "\n====================================================" << std::endl;
			std::cerr << "====================  TEST INFO  ===================" << std::endl;
			std::cerr << "====================================================\n" << std::endl;
			std::cerr << description << std::endl;
		}
		std::cerr << "\n====================================================" << std::endl;
		std::cerr << "================  FAILURES SUMMARY  ================" << std::endl;
		std::cerr << "====================================================\n" << std::endl;
		std::cerr << failures_summary;
	}

	if (DeleteTestPath()) {
		TestDeleteDirectory(dir);
	}

	return result;
}
