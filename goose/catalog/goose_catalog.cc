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


#include <goose/catalog/goose_catalog.h>
#include <goose/catalog/dependency_manager.h>
#include <goose/catalog/catalog_entry/goose_schema_entry.h>
#include <goose/storage/storage_manager.h>
#include <goose/parser/parsed_data/drop_info.h>
#include <goose/parser/parsed_data/create_schema_info.h>
#include <goose/catalog/default/default_schemas.h>
#include <goose/function/built_in_functions.h>
#include <goose/main/attached_database.h>
#include <goose/transaction/goose_transaction_manager.h>
#include <goose/function/function_list.h>
#include <goose/common/encryption_state.h>

namespace goose {
    GooseCatalog::GooseCatalog(AttachedDatabase &db)
        : Catalog(db), dependency_manager(make_uniq<DependencyManager>(*this)),
          schemas(make_uniq<CatalogSet>(*this, IsSystemCatalog()
                                                   ? make_uniq<DefaultSchemaGenerator>(*this)
                                                   : nullptr)) {
    }

    GooseCatalog::~GooseCatalog() {
    }

    void GooseCatalog::Initialize(bool load_builtin) {
        // first initialize the base system catalogs
        // these are never written to the WAL
        // we start these at 1 because deleted entries default to 0
        auto data = CatalogTransaction::GetSystemTransaction(GetDatabase());

        // create the default schema
        CreateSchemaInfo info;
        info.schema = DEFAULT_SCHEMA;
        info.internal = true;
        info.on_conflict = OnCreateConflict::IGNORE_ON_CONFLICT;
        CreateSchema(data, info);

        if (load_builtin) {
            BuiltinFunctions builtin(data, *this);
            builtin.Initialize();

            // initialize default functions
            FunctionList::RegisterFunctions(*this, data);
        }

        Verify();
    }

    bool GooseCatalog::IsGooseCatalog() {
        return true;
    }

    optional_ptr<DependencyManager> GooseCatalog::GetDependencyManager() {
        return dependency_manager.get();
    }

    //===--------------------------------------------------------------------===//
    // Schema
    //===--------------------------------------------------------------------===//
    optional_ptr<CatalogEntry> GooseCatalog::CreateSchemaInternal(CatalogTransaction transaction,
                                                                  CreateSchemaInfo &info) {
        LogicalDependencyList dependencies;

        if (!info.internal && DefaultSchemaGenerator::IsDefaultSchema(info.schema)) {
            return nullptr;
        }
        auto entry = make_uniq<GooseSchemaEntry>(*this, info);
        auto result = entry.get();
        if (!schemas->CreateEntry(transaction, info.schema, std::move(entry), dependencies)) {
            return nullptr;
        }
        return result;
    }

    optional_ptr<CatalogEntry> GooseCatalog::CreateSchema(CatalogTransaction transaction, CreateSchemaInfo &info) {
        D_ASSERT(!info.schema.empty());
        auto result = CreateSchemaInternal(transaction, info);
        if (!result) {
            switch (info.on_conflict) {
                case OnCreateConflict::ERROR_ON_CONFLICT:
                    throw CatalogException::EntryAlreadyExists(CatalogType::SCHEMA_ENTRY, info.schema);
                case OnCreateConflict::REPLACE_ON_CONFLICT: {
                    DropInfo drop_info;
                    drop_info.type = CatalogType::SCHEMA_ENTRY;
                    drop_info.catalog = info.catalog;
                    drop_info.name = info.schema;
                    DropSchema(transaction, drop_info);
                    result = CreateSchemaInternal(transaction, info);
                    if (!result) {
                        throw InternalException("Failed to create schema entry in CREATE_OR_REPLACE");
                    }
                    break;
                }
                case OnCreateConflict::IGNORE_ON_CONFLICT:
                    break;
                default:
                    throw InternalException("Unsupported OnCreateConflict for CreateSchema");
            }
            return nullptr;
        }
        return result;
    }

    void GooseCatalog::DropSchema(CatalogTransaction transaction, DropInfo &info) {
        D_ASSERT(!info.name.empty());
        if (!schemas->DropEntry(transaction, info.name, info.cascade)) {
            if (info.if_not_found == OnEntryNotFound::THROW_EXCEPTION) {
                throw CatalogException::MissingEntry(CatalogType::SCHEMA_ENTRY, info.name, string());
            }
        }
    }

    void GooseCatalog::DropSchema(ClientContext &context, DropInfo &info) {
        DropSchema(GetCatalogTransaction(context), info);
    }

    void GooseCatalog::ScanSchemas(ClientContext &context, std::function<void(SchemaCatalogEntry &)> callback) {
        schemas->Scan(GetCatalogTransaction(context),
                      [&](CatalogEntry &entry) { callback(entry.Cast<SchemaCatalogEntry>()); });
    }

    void GooseCatalog::ScanSchemas(std::function<void(SchemaCatalogEntry &)> callback) {
        schemas->Scan([&](CatalogEntry &entry) { callback(entry.Cast<SchemaCatalogEntry>()); });
    }

    CatalogSet &GooseCatalog::GetSchemaCatalogSet() {
        return *schemas;
    }

    optional_ptr<SchemaCatalogEntry> GooseCatalog::LookupSchema(CatalogTransaction transaction,
                                                                const EntryLookupInfo &schema_lookup,
                                                                OnEntryNotFound if_not_found) {
        auto &schema_name = schema_lookup.GetEntryName();
        D_ASSERT(!schema_name.empty());
        auto entry = schemas->GetEntry(transaction, schema_name);
        if (!entry) {
            if (if_not_found == OnEntryNotFound::THROW_EXCEPTION) {
                throw CatalogException(schema_lookup.GetErrorContext(), "Schema with name %s does not exist!",
                                       schema_name);
            }
            return nullptr;
        }
        return &entry->Cast<SchemaCatalogEntry>();
    }

    DatabaseSize GooseCatalog::GetDatabaseSize(ClientContext &context) {
        auto &transaction = GooseTransactionManager::Get(db);
        auto lock = transaction.SharedCheckpointLock();
        return db.GetStorageManager().GetDatabaseSize();
    }

    vector<MetadataBlockInfo> GooseCatalog::GetMetadataInfo(ClientContext &context) {
        auto &transaction = GooseTransactionManager::Get(db);
        auto lock = transaction.SharedCheckpointLock();
        return db.GetStorageManager().GetMetadataInfo();
    }

    bool GooseCatalog::InMemory() {
        return db.GetStorageManager().InMemory();
    }

    string GooseCatalog::GetDBPath() {
        return db.GetStorageManager().GetDBPath();
    }

    bool GooseCatalog::IsEncrypted() const {
        return IsSystemCatalog() ? false : db.GetStorageManager().IsEncrypted();
    }

    string GooseCatalog::GetEncryptionCipher() const {
        return IsSystemCatalog() ? string() : EncryptionTypes::CipherToString(db.GetStorageManager().GetCipher());
    }

    void GooseCatalog::Verify() {
#ifdef DEBUG
        Catalog::Verify();
        schemas->Verify(*this);
#endif
    }

    optional_idx GooseCatalog::GetCatalogVersion(ClientContext &context) {
        auto &transaction_manager = GooseTransactionManager::Get(db);
        auto transaction = GetCatalogTransaction(context);
        D_ASSERT(transaction.transaction);
        return transaction_manager.GetCatalogVersion(*transaction.transaction);
    }

    //===--------------------------------------------------------------------===//
    // Encryption
    //===--------------------------------------------------------------------===//
    void GooseCatalog::SetEncryptionKeyId(const string &key_id) {
        encryption_key_id = key_id;
    }

    string &GooseCatalog::GetEncryptionKeyId() {
        return encryption_key_id;
    }

    void GooseCatalog::SetIsEncrypted() {
        is_encrypted = true;
    }

    bool GooseCatalog::GetIsEncrypted() {
        return is_encrypted;
    }
} // namespace goose
