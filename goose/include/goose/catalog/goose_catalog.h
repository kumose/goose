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

#include <goose/catalog/catalog.h>

namespace goose {
    //! The Catalog object represents the catalog of the database.
    class GooseCatalog : public Catalog {
    public:
        explicit GooseCatalog(AttachedDatabase &db);

        ~GooseCatalog() override;

    public:
        bool IsGooseCatalog() override;

        void Initialize(bool load_builtin) override;

        string GetCatalogType() override {
            return "goose";
        }

        mutex &GetWriteLock() {
            return write_lock;
        }

        // Encryption Functions
        void SetEncryptionKeyId(const string &key_id);

        string &GetEncryptionKeyId();

        void SetIsEncrypted();

        bool GetIsEncrypted();

    public:
        GOOSE_API optional_ptr<CatalogEntry>
        CreateSchema(CatalogTransaction transaction, CreateSchemaInfo &info) override;

        GOOSE_API void ScanSchemas(ClientContext &context, std::function<void(SchemaCatalogEntry &)> callback) override;

        GOOSE_API void ScanSchemas(std::function<void(SchemaCatalogEntry &)> callback);

        GOOSE_API optional_ptr<SchemaCatalogEntry> LookupSchema(CatalogTransaction transaction,
                                                                const EntryLookupInfo &schema_lookup,
                                                                OnEntryNotFound if_not_found) override;

        GOOSE_API PhysicalOperator &PlanCreateTableAs(ClientContext &context, PhysicalPlanGenerator &planner,
                                                      LogicalCreateTable &op, PhysicalOperator &plan) override;

        GOOSE_API PhysicalOperator &PlanInsert(ClientContext &context, PhysicalPlanGenerator &planner,
                                               LogicalInsert &op,
                                               optional_ptr<PhysicalOperator> plan) override;

        GOOSE_API PhysicalOperator &PlanDelete(ClientContext &context, PhysicalPlanGenerator &planner,
                                               LogicalDelete &op,
                                               PhysicalOperator &plan) override;

        GOOSE_API PhysicalOperator &PlanUpdate(ClientContext &context, PhysicalPlanGenerator &planner,
                                               LogicalUpdate &op,
                                               PhysicalOperator &plan) override;

        GOOSE_API PhysicalOperator &PlanMergeInto(ClientContext &context, PhysicalPlanGenerator &planner,
                                                  LogicalMergeInto &op, PhysicalOperator &plan) override;

        GOOSE_API unique_ptr<LogicalOperator> BindCreateIndex(Binder &binder, CreateStatement &stmt,
                                                              TableCatalogEntry &table,
                                                              unique_ptr<LogicalOperator> plan) override;

        GOOSE_API unique_ptr<LogicalOperator> BindAlterAddIndex(Binder &binder, TableCatalogEntry &table_entry,
                                                                unique_ptr<LogicalOperator> plan,
                                                                unique_ptr<CreateIndexInfo> create_info,
                                                                unique_ptr<AlterTableInfo> alter_info) override;

        CatalogSet &GetSchemaCatalogSet();

        DatabaseSize GetDatabaseSize(ClientContext &context) override;

        vector<MetadataBlockInfo> GetMetadataInfo(ClientContext &context) override;

        GOOSE_API bool InMemory() override;

        GOOSE_API string GetDBPath() override;

        GOOSE_API bool IsEncrypted() const override;

        GOOSE_API string GetEncryptionCipher() const override;

        GOOSE_API optional_idx GetCatalogVersion(ClientContext &context) override;

        optional_ptr<DependencyManager> GetDependencyManager() override;

    private:
        GOOSE_API void DropSchema(CatalogTransaction transaction, DropInfo &info);

        GOOSE_API void DropSchema(ClientContext &context, DropInfo &info) override;

        optional_ptr<CatalogEntry> CreateSchemaInternal(CatalogTransaction transaction, CreateSchemaInfo &info);

        void Verify() override;

    private:
        //! The DependencyManager manages dependencies between different catalog objects
        unique_ptr<DependencyManager> dependency_manager;
        //! Write lock for the catalog
        mutex write_lock;
        //! The catalog set holding the schemas
        unique_ptr<CatalogSet> schemas;

        //! Identifies whether the db is encrypted
        bool is_encrypted = false;
        //! If is encrypted, store the encryption key_id
        string encryption_key_id;
    };
} // namespace goose
