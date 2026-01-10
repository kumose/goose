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
