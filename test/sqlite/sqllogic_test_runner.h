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

#include <goose/goose.h>
#include <goose/common/types-import.h>
#include "sqllogic_command.h"
#include "test_config.h"

namespace goose {

class Command;
class LoopCommand;
class SQLLogicParser;

enum class RequireResult { PRESENT, MISSING };

struct CachedLabelData {
public:
	CachedLabelData(const string &hash, unique_ptr<QueryResult> result) : hash(hash), result(std::move(result)) {
	}

public:
	string hash;
	unique_ptr<QueryResult> result;
};

struct HashLabelMap {
public:
	void WithLock(std::function<void(unordered_map<string, CachedLabelData> &map)> cb) {
		std::lock_guard<std::mutex> guard(lock);
		cb(map);
	}

public:
	std::mutex lock;
	unordered_map<string, CachedLabelData> map;
};

class SQLLogicTestRunner {
public:
	SQLLogicTestRunner(string dbpath);
	~SQLLogicTestRunner();

	string file_name;
	string dbpath;
	vector<string> loaded_databases;
	goose::unique_ptr<Goose> db;
	goose::unique_ptr<Connection> con;
	goose::unique_ptr<DBConfig> config;
	unordered_set<string> extensions;
	unordered_map<string, goose::unique_ptr<Connection>> named_connection_map;
	bool output_hash_mode = false;
	bool output_result_mode = false;
	bool debug_mode = false;
	atomic<bool> finished_processing_file;
	int32_t hash_threshold = 0;
	vector<LoopCommand *> active_loops;
	goose::unique_ptr<Command> top_level_loop;
	bool original_sqlite_test = false;
	bool output_sql = false;
	bool enable_verification = false;
	bool skip_reload = false;
	unordered_map<string, string> environment_variables;
	string local_extension_repo;
	TestConfiguration::ExtensionAutoLoadingMode autoloading_mode;
	bool autoinstall_is_checked;

	// If these error msgs occur in a test, the test will abort but still count as passed
	unordered_set<string> ignore_error_messages = {"HTTP", "Unable to connect"};
	// If these error msgs occur a statement that is expected to fail, the test will fail
	unordered_set<string> always_fail_error_messages = {"differs from original result!", "INTERNAL"};

	//! The map converting the labels to the hash values
	HashLabelMap hash_label_map;
	mutex log_lock;

public:
	void ExecuteFile(string script);
	virtual void LoadDatabase(string dbpath, bool load_extensions);

	string ReplaceKeywords(string input);

	bool InLoop() {
		return !active_loops.empty();
	}
	void ExecuteCommand(unique_ptr<Command> command);
	void Reconnect();
	void StartLoop(LoopDefinition loop);
	void EndLoop();
	string ReplaceLoopIterator(string text, string loop_iterator_name, string replacement);
	string LoopReplacement(string text, const vector<LoopDefinition> &loops);
	bool ForEachTokenReplace(const string &parameter, vector<string> &result);
	static ExtensionLoadResult LoadExtension(Goose &db, const std::string &extension);

private:
	RequireResult CheckRequire(SQLLogicParser &parser, const vector<string> &params);
};

} // namespace goose
