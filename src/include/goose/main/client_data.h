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

#include <goose/common/case_insensitive_map.h>
#include <goose/common/common.h>

namespace goose {

class AttachedDatabase;
class BufferedFileWriter;
class ClientContext;
class CatalogSearchPath;
class FileOpener;
class FileSystem;
class QueryProfiler;
class PreparedStatementData;
class RandomEngine;
class BufferManager;

struct ClientData {
public:
	explicit ClientData(ClientContext &context);
	~ClientData();

public:
	//! The query profiler of this client.
	shared_ptr<QueryProfiler> profiler;

	//! The set of temporary objects belonging to this client.
	shared_ptr<AttachedDatabase> temporary_objects;
	//! The set of bound prepared statements belonging to this client.
	case_insensitive_map_t<shared_ptr<PreparedStatementData>> prepared_statements;

	//! The random generator used by random().
	//! Its seed value can be set by setseed().
	unique_ptr<RandomEngine> random_engine;

	//! The catalog search path.
	unique_ptr<CatalogSearchPath> catalog_search_path;
	//! The file search path.
	string file_search_path;

	//! The file opener of the client context.
	unique_ptr<FileOpener> file_opener;
	//! The clients' file system wrapper.
	unique_ptr<FileSystem> client_file_system;
	//! The clients' buffer manager wrapper.
	unique_ptr<BufferManager> client_buffer_manager;

	//! The Max Line Length Size of Last Query Executed on a CSV File. (Only used for testing)
	//! FIXME: this should not be done like this
	bool debug_set_max_line_length = false;
	idx_t debug_max_line_length = 0;

	//! The writer used to log queries, if logging is enabled.
	//! DEPRECATED: Now, queries are written to the generic log.
	unique_ptr<BufferedFileWriter> log_query_writer;

public:
	GOOSE_API static ClientData &Get(ClientContext &context);
	GOOSE_API static const ClientData &Get(const ClientContext &context);
};

} // namespace goose
