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

#pragma once

#include <goose/common/common.h>
#include <goose/common/enums/access_mode.h>
#include <goose/parser/tableref/table_function_ref.h>
#include <goose/storage/storage_manager.h>

namespace goose {
class AttachedDatabase;
struct AttachInfo;
class Catalog;
class TransactionManager;

//! The StorageExtensionInfo holds static information relevant to the storage extension
struct StorageExtensionInfo {
	virtual ~StorageExtensionInfo() {
	}
};

typedef unique_ptr<Catalog> (*attach_function_t)(optional_ptr<StorageExtensionInfo> storage_info,
                                                 ClientContext &context, AttachedDatabase &db, const string &name,
                                                 AttachInfo &info, AttachOptions &options);
typedef unique_ptr<TransactionManager> (*create_transaction_manager_t)(optional_ptr<StorageExtensionInfo> storage_info,
                                                                       AttachedDatabase &db, Catalog &catalog);

class StorageExtension {
public:
	attach_function_t attach;
	create_transaction_manager_t create_transaction_manager;

	//! Additional info passed to the various storage functions
	shared_ptr<StorageExtensionInfo> storage_info;

	virtual ~StorageExtension() {
	}

	virtual void OnCheckpointStart(AttachedDatabase &db, CheckpointOptions checkpoint_options) {
	}

	virtual void OnCheckpointEnd(AttachedDatabase &db, CheckpointOptions checkpoint_options) {
	}
};

struct OpenFileStorageExtension {
	static unique_ptr<StorageExtension> Create();
};

} // namespace goose
