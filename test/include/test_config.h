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
#include <goose/common/optional_idx.h>
#include <goose/common/enums/debug_vector_verification.h>
#include <goose/common/types/value.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/enums/debug_initialize.h>
#include <sys/types.h>
#include <unordered_map>

namespace goose {

enum class SortStyle : uint8_t { NO_SORT, ROW_SORT, VALUE_SORT };

struct ConfigSetting {
	string name;
	Value value;
};

class TestConfiguration {
public:
	enum class ExtensionAutoLoadingMode { NONE = 0, AVAILABLE = 1, ALL = 2 };

	enum class SelectPolicy : uint8_t {
		NONE,   // does not match any explicit policy (default: policy=SELECT)
		SELECT, // matches explicit select
		SKIP    // matches explicit skip
	};

	static TestConfiguration &Get();

	void Initialize();
	bool ParseArgument(const string &arg, idx_t argc, char **argv, idx_t &i);
	bool TryParseOption(const string &name, const Value &value);
	void ParseOption(const string &name, const Value &value);
	void LoadConfig(const string &config_path);

	void UpdateEnvironment();
	string GetWorkingDirectory();
	bool ChangeWorkingDirectory(const string &dir); // true -> changed

	void ProcessPath(string &path, const string &test_name);

	string GetDescription();
	string GetInitialDBPath();
	optional_idx GetMaxThreads();
	optional_idx GetBlockAllocSize();
	idx_t GetCheckpointWALSize();
	bool GetForceRestart();
	bool GetCheckpointOnShutdown();
	bool GetTestMemoryLeaks();
	bool RunStorageFuzzer();
	bool GetSummarizeFailures();
	bool GetSkipCompiledTests();
	DebugVectorVerification GetVectorVerification();
	DebugInitialize GetDebugInitialize();
	ExtensionAutoLoadingMode GetExtensionAutoLoadingMode();
	bool ShouldSkipTest(const string &test_name);
	string DataLocation();
	string OnInitCommand();
	string OnLoadCommand();
	string OnConnectionCommand();
	string OnCleanupCommand();
	SortStyle GetDefaultSortStyle();
	vector<string> ExtensionToBeLoadedOnLoad();
	vector<string> ErrorMessagesToBeSkipped();
	string GetStorageVersion();
	string GetTestEnv(const string &key, const string &default_value);
	const unordered_map<string, string> &GetTestEnvMap();
	vector<unordered_set<string>> GetSelectTagSets();
	vector<unordered_set<string>> GetSkipTagSets();
	SelectPolicy GetPolicyForTagSet(const vector<string> &tag_set);
	vector<ConfigSetting> GetConfigSettings();

	static bool TestForceStorage();
	static bool TestForceReload();
	static bool TestMemoryLeaks();
	static bool TestRunStorageFuzzer();

	static void LoadBaseConfig(const Value &input);
	static void ParseConnectScript(const Value &input);
	static void CheckSortStyle(const Value &input);
	static bool TryParseSortStyle(const string &sort_style, SortStyle &result);
	static void AppendSelectTagSet(const Value &tag_set);
	static void AppendSkipTagSet(const Value &tag_set);

private:
	case_insensitive_map_t<Value> options;
	unordered_set<string> tests_to_be_skipped;

	// explicitly take ownership of working_dir here, giving runners an API to chdir,
	// and get env updates to match
	string working_dir;
	string test_uuid;
	unordered_map<string, string> test_env;

	vector<unordered_set<string>> select_tag_sets;
	vector<unordered_set<string>> skip_tag_sets;

private:
	template <class T, class VAL_T = T>
	T GetOptionOrDefault(const string &name, T default_val);

	static string ReadFileToString(const string &path);
};

class FailureSummary {
public:
	FailureSummary();

	static void Log(string message);
	static string GetFailureSummary();
	static idx_t GetSummaryCounter();
	static bool SkipLoggingSameError(const string &file_name);

private:
	static FailureSummary &Instance();
	bool SkipLoggingSameErrorInternal(const string &file_name);

private:
	mutex failures_lock;
	atomic<idx_t> failures_summary_counter;
	vector<string> failures_summary;
	set<string> reported_files;
};

} // namespace goose
