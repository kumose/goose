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

#include <goose/catalog/catalog_entry/goose_schema_entry.h>

#include <goose/catalog/catalog_entry/aggregate_function_catalog_entry.h>
#include <goose/catalog/catalog_entry/collate_catalog_entry.h>
#include <goose/catalog/catalog_entry/copy_function_catalog_entry.h>
#include <goose/catalog/catalog_entry/goose_index_entry.h>
#include <goose/catalog/catalog_entry/goose_table_entry.h>
#include <goose/catalog/catalog_entry/pragma_function_catalog_entry.h>
#include <goose/catalog/catalog_entry/scalar_function_catalog_entry.h>
#include <goose/catalog/catalog_entry/scalar_macro_catalog_entry.h>
#include <goose/catalog/catalog_entry/sequence_catalog_entry.h>
#include <goose/catalog/catalog_entry/table_catalog_entry.h>
#include <goose/catalog/catalog_entry/table_function_catalog_entry.h>
#include <goose/catalog/catalog_entry/table_macro_catalog_entry.h>
#include <goose/catalog/catalog_entry/type_catalog_entry.h>
#include <goose/catalog/catalog_entry/view_catalog_entry.h>
#include <goose/catalog/default/default_functions.h>
#include <goose/catalog/default/default_table_functions.h>
#include <goose/catalog/default/default_types.h>
#include <goose/catalog/default/default_views.h>
#include <goose/catalog/dependency_list.h>
#include <goose/main/attached_database.h>
#include <goose/main/database.h>
#include <goose/parser/constraints/foreign_key_constraint.h>
#include <goose/parser/parsed_data/alter_table_info.h>
#include <goose/parser/parsed_data/create_collation_info.h>
#include <goose/parser/parsed_data/create_copy_function_info.h>
#include <goose/parser/parsed_data/create_index_info.h>
#include <goose/parser/parsed_data/create_pragma_function_info.h>
#include <goose/parser/parsed_data/create_scalar_function_info.h>
#include <goose/parser/parsed_data/create_schema_info.h>
#include <goose/parser/parsed_data/create_sequence_info.h>
#include <goose/parser/parsed_data/create_table_function_info.h>
#include <goose/parser/parsed_data/create_type_info.h>
#include <goose/parser/parsed_data/create_view_info.h>
#include <goose/parser/parsed_data/drop_info.h>
#include <goose/planner/constraints/bound_foreign_key_constraint.h>
#include <goose/planner/parsed_data/bound_create_table_info.h>
#include <goose/storage/data_table.h>
#include <goose/transaction/goose_transaction.h>
#include <goose/transaction/meta_transaction.h>

namespace goose {
    static void FindForeignKeyInformation(TableCatalogEntry &table, AlterForeignKeyType alter_fk_type,
                                          vector<unique_ptr<AlterForeignKeyInfo> > &fk_arrays) {
        auto &constraints = table.GetConstraints();
        auto &catalog = table.ParentCatalog();
        auto &name = table.name;
        for (idx_t i = 0; i < constraints.size(); i++) {
            auto &cond = constraints[i];
            if (cond->type != ConstraintType::FOREIGN_KEY) {
                continue;
            }
            auto &fk = cond->Cast<ForeignKeyConstraint>();
            if (fk.info.type == ForeignKeyType::FK_TYPE_FOREIGN_KEY_TABLE) {
                AlterEntryData alter_data(catalog.GetName(), fk.info.schema, fk.info.table,
                                          OnEntryNotFound::THROW_EXCEPTION);
                fk_arrays.push_back(make_uniq<AlterForeignKeyInfo>(std::move(alter_data), name, fk.pk_columns,
                                                                   fk.fk_columns, fk.info.pk_keys, fk.info.fk_keys,
                                                                   alter_fk_type));
            } else if (fk.info.type == ForeignKeyType::FK_TYPE_PRIMARY_KEY_TABLE &&
                       alter_fk_type == AlterForeignKeyType::AFT_DELETE) {
                throw CatalogException(
                    "Could not drop the table because this table is main key table of the table \"%s\"",
                    fk.info.table);
            }
        }
    }

    GooseSchemaEntry::GooseSchemaEntry(Catalog &catalog, CreateSchemaInfo &info)
        : SchemaCatalogEntry(catalog, info),
          tables(catalog, catalog.IsSystemCatalog() ? make_uniq<DefaultViewGenerator>(catalog, *this) : nullptr),
          indexes(catalog),
          table_functions(catalog,
                          catalog.IsSystemCatalog()
                              ? make_uniq<DefaultTableFunctionGenerator>(catalog, *this)
                              : nullptr),
          copy_functions(catalog), pragma_functions(catalog),
          functions(catalog, catalog.IsSystemCatalog() ? make_uniq<DefaultFunctionGenerator>(catalog, *this) : nullptr),
          sequences(catalog), collations(catalog), types(catalog, make_uniq<DefaultTypeGenerator>(catalog, *this)) {
    }

    unique_ptr<CatalogEntry> GooseSchemaEntry::Copy(ClientContext &context) const {
        auto info_copy = GetInfo();
        auto &cast_info = info_copy->Cast<CreateSchemaInfo>();

        auto result = make_uniq<GooseSchemaEntry>(catalog, cast_info);

        return result;
    }

    optional_ptr<CatalogEntry> GooseSchemaEntry::AddEntryInternal(CatalogTransaction transaction,
                                                                  unique_ptr<StandardEntry> entry,
                                                                  OnCreateConflict on_conflict,
                                                                  LogicalDependencyList dependencies) {
        auto entry_name = entry->name;
        auto entry_type = entry->type;
        auto result = entry.get();

        if (transaction.context) {
            auto &meta = MetaTransaction::Get(transaction.GetContext());
            auto modified_database = meta.ModifiedDatabase();
            auto &db = ParentCatalog().GetAttached();
            if (!db.IsTemporary() && !db.IsSystem()) {
                if (!modified_database || !RefersToSameObject(*modified_database, ParentCatalog().GetAttached())) {
                    throw InternalException(
                        "GooseSchemaEntry::AddEntryInternal called but this database is not marked as modified");
                }
            }
        }
        // first find the set for this entry
        auto &set = GetCatalogSet(entry_type);
        dependencies.AddDependency(*this);
        if (on_conflict == OnCreateConflict::IGNORE_ON_CONFLICT) {
            auto old_entry = set.GetEntry(transaction, entry_name);
            if (old_entry) {
                return nullptr;
            }
        }

        if (on_conflict == OnCreateConflict::REPLACE_ON_CONFLICT) {
            // CREATE OR REPLACE: first try to drop the entry
            auto old_entry = set.GetEntry(transaction, entry_name);
            if (old_entry) {
                if (dependencies.Contains(*old_entry)) {
                    throw CatalogException("CREATE OR REPLACE is not allowed to depend on itself");
                }
                if (old_entry->type != entry_type) {
                    throw CatalogException("Existing object %s is of type %s, trying to replace with type %s",
                                           entry_name,
                                           CatalogTypeToString(old_entry->type), CatalogTypeToString(entry_type));
                }
                OnDropEntry(transaction, *old_entry);
                (void) set.DropEntry(transaction, entry_name, false, entry->internal);
            }
        }
        // now try to add the entry
        if (!set.CreateEntry(transaction, entry_name, std::move(entry), dependencies)) {
            // entry already exists!
            if (on_conflict == OnCreateConflict::ERROR_ON_CONFLICT) {
                throw CatalogException::EntryAlreadyExists(entry_type, entry_name);
            } else {
                return nullptr;
            }
        }
        return result;
    }

    optional_ptr<CatalogEntry>
    GooseSchemaEntry::CreateTable(CatalogTransaction transaction, BoundCreateTableInfo &info) {
        auto table = make_uniq<GooseTableEntry>(catalog, *this, info);

        // add a foreign key constraint in main key table if there is a foreign key constraint
        vector<unique_ptr<AlterForeignKeyInfo> > fk_arrays;
        FindForeignKeyInformation(*table, AlterForeignKeyType::AFT_ADD, fk_arrays);
        for (idx_t i = 0; i < fk_arrays.size(); i++) {
            // alter primary key table
            auto &fk_info = *fk_arrays[i];
            Alter(transaction, fk_info);

            // make a dependency between this table and referenced table
            auto &set = GetCatalogSet(CatalogType::TABLE_ENTRY);
            info.dependencies.AddDependency(*set.GetEntry(transaction, fk_info.name));
        }
        for (auto &dep: info.dependencies.Set()) {
            table->dependencies.AddDependency(dep);
        }

        auto entry = AddEntryInternal(transaction, std::move(table), info.Base().on_conflict, info.dependencies);
        if (!entry) {
            return nullptr;
        }

        return entry;
    }

    optional_ptr<CatalogEntry> GooseSchemaEntry::CreateFunction(CatalogTransaction transaction,
                                                                CreateFunctionInfo &info) {
        if (info.on_conflict == OnCreateConflict::ALTER_ON_CONFLICT) {
            // check if the original entry exists
            auto &catalog_set = GetCatalogSet(info.type);
            auto current_entry = catalog_set.GetEntry(transaction, info.name);
            if (current_entry) {
                // the current entry exists - alter it instead
                auto alter_info = info.GetAlterInfo();
                Alter(transaction, *alter_info);
                return nullptr;
            }
        }
        unique_ptr<StandardEntry> function;
        switch (info.type) {
            case CatalogType::SCALAR_FUNCTION_ENTRY:
                function = make_uniq_base<StandardEntry, ScalarFunctionCatalogEntry>(catalog, *this,
                    info.Cast<CreateScalarFunctionInfo>());
                break;
            case CatalogType::TABLE_FUNCTION_ENTRY:
                function = make_uniq_base<StandardEntry, TableFunctionCatalogEntry>(catalog, *this,
                    info.Cast<CreateTableFunctionInfo>());
                break;
            case CatalogType::MACRO_ENTRY:
                // create a macro function
                function = make_uniq_base<StandardEntry, ScalarMacroCatalogEntry>(
                    catalog, *this, info.Cast<CreateMacroInfo>());
                break;

            case CatalogType::TABLE_MACRO_ENTRY:
                // create a macro table function
                function = make_uniq_base<StandardEntry, TableMacroCatalogEntry>(
                    catalog, *this, info.Cast<CreateMacroInfo>());
                break;
            case CatalogType::AGGREGATE_FUNCTION_ENTRY:
                D_ASSERT(info.type == CatalogType::AGGREGATE_FUNCTION_ENTRY);
                // create an aggregate function
                function = make_uniq_base<StandardEntry, AggregateFunctionCatalogEntry>(
                    catalog, *this, info.Cast<CreateAggregateFunctionInfo>());
                break;
            default:
                throw InternalException("Unknown function type \"%s\"", CatalogTypeToString(info.type));
        }
        function->internal = info.internal;
        return AddEntry(transaction, std::move(function), info.on_conflict);
    }

    optional_ptr<CatalogEntry> GooseSchemaEntry::AddEntry(CatalogTransaction transaction,
                                                          unique_ptr<StandardEntry> entry,
                                                          OnCreateConflict on_conflict) {
        LogicalDependencyList dependencies = entry->dependencies;
        return AddEntryInternal(transaction, std::move(entry), on_conflict, dependencies);
    }

    optional_ptr<CatalogEntry> GooseSchemaEntry::CreateSequence(CatalogTransaction transaction,
                                                                CreateSequenceInfo &info) {
        auto sequence = make_uniq<SequenceCatalogEntry>(catalog, *this, info);
        return AddEntry(transaction, std::move(sequence), info.on_conflict);
    }

    optional_ptr<CatalogEntry> GooseSchemaEntry::CreateType(CatalogTransaction transaction, CreateTypeInfo &info) {
        auto type_entry = make_uniq<TypeCatalogEntry>(catalog, *this, info);
        return AddEntry(transaction, std::move(type_entry), info.on_conflict);
    }

    optional_ptr<CatalogEntry> GooseSchemaEntry::CreateView(CatalogTransaction transaction, CreateViewInfo &info) {
        auto view = make_uniq<ViewCatalogEntry>(catalog, *this, info);
        return AddEntry(transaction, std::move(view), info.on_conflict);
    }

    optional_ptr<CatalogEntry> GooseSchemaEntry::CreateIndex(CatalogTransaction transaction, CreateIndexInfo &info,
                                                             TableCatalogEntry &table) {
        info.dependencies.AddDependency(table);

        // currently, we can not alter PK/FK/UNIQUE constraints
        // concurrency-safe name checks against other INDEX catalog entries happens in the catalog
        if (info.on_conflict != OnCreateConflict::IGNORE_ON_CONFLICT &&
            !table.GetStorage().IndexNameIsUnique(info.index_name)) {
            throw CatalogException("An index with the name " + info.index_name + " already exists!");
        }

        auto index = make_uniq<GooseIndexEntry>(catalog, *this, info, table);
        auto dependencies = index->dependencies;
        return AddEntryInternal(transaction, std::move(index), info.on_conflict, dependencies);
    }

    optional_ptr<CatalogEntry> GooseSchemaEntry::CreateCollation(CatalogTransaction transaction,
                                                                 CreateCollationInfo &info) {
        auto collation = make_uniq<CollateCatalogEntry>(catalog, *this, info);
        collation->internal = info.internal;
        return AddEntry(transaction, std::move(collation), info.on_conflict);
    }

    optional_ptr<CatalogEntry> GooseSchemaEntry::CreateTableFunction(CatalogTransaction transaction,
                                                                     CreateTableFunctionInfo &info) {
        auto table_function = make_uniq<TableFunctionCatalogEntry>(catalog, *this, info);
        table_function->internal = info.internal;
        return AddEntry(transaction, std::move(table_function), info.on_conflict);
    }

    optional_ptr<CatalogEntry> GooseSchemaEntry::CreateCopyFunction(CatalogTransaction transaction,
                                                                    CreateCopyFunctionInfo &info) {
        auto copy_function = make_uniq<CopyFunctionCatalogEntry>(catalog, *this, info);
        copy_function->internal = info.internal;
        return AddEntry(transaction, std::move(copy_function), info.on_conflict);
    }

    optional_ptr<CatalogEntry> GooseSchemaEntry::CreatePragmaFunction(CatalogTransaction transaction,
                                                                      CreatePragmaFunctionInfo &info) {
        auto pragma_function = make_uniq<PragmaFunctionCatalogEntry>(catalog, *this, info);
        pragma_function->internal = info.internal;
        return AddEntry(transaction, std::move(pragma_function), info.on_conflict);
    }

    void GooseSchemaEntry::Alter(CatalogTransaction transaction, AlterInfo &info) {
        CatalogType type = info.GetCatalogType();

        auto &set = GetCatalogSet(type);
        if (info.type == AlterType::CHANGE_OWNERSHIP) {
            if (!set.AlterOwnership(transaction, info.Cast<ChangeOwnershipInfo>())) {
                throw CatalogException("Couldn't change ownership!");
            }
        } else {
            string name = info.name;
            if (!set.AlterEntry(transaction, name, info)) {
                throw CatalogException::MissingEntry(type, name, string());
            }
        }
    }

    void GooseSchemaEntry::Scan(ClientContext &context, CatalogType type,
                                const std::function<void(CatalogEntry &)> &callback) {
        auto &set = GetCatalogSet(type);
        set.Scan(GetCatalogTransaction(context), callback);
    }

    void GooseSchemaEntry::Scan(CatalogType type, const std::function<void(CatalogEntry &)> &callback) {
        auto &set = GetCatalogSet(type);
        set.Scan(callback);
    }

    void GooseSchemaEntry::DropEntry(ClientContext &context, DropInfo &info) {
        auto &set = GetCatalogSet(info.type);

        // first find the entry
        auto transaction = GetCatalogTransaction(context);
        auto existing_entry = set.GetEntry(transaction, info.name);
        if (!existing_entry) {
            throw InternalException("Failed to drop entry \"%s\" - entry could not be found", info.name);
        }
        if (existing_entry->type != info.type) {
            throw CatalogException("Existing object %s is of type %s, trying to drop type %s", info.name,
                                   CatalogTypeToString(existing_entry->type), CatalogTypeToString(info.type));
        }

        vector<unique_ptr<AlterForeignKeyInfo> > fk_arrays;
        if (existing_entry->type == CatalogType::TABLE_ENTRY) {
            // if there is a foreign key constraint, get that information
            auto &table_entry = existing_entry->Cast<TableCatalogEntry>();
            FindForeignKeyInformation(table_entry, AlterForeignKeyType::AFT_DELETE, fk_arrays);
        }

        OnDropEntry(transaction, *existing_entry);
        if (!set.DropEntry(transaction, info.name, info.cascade, info.allow_drop_internal)) {
            throw InternalException("Could not drop element because of an internal error");
        }

        // remove the foreign key constraint in main key table if main key table's name is valid
        for (idx_t i = 0; i < fk_arrays.size(); i++) {
            // alter primary key table
            Alter(transaction, *fk_arrays[i]);
        }
    }

    void GooseSchemaEntry::OnDropEntry(CatalogTransaction transaction, CatalogEntry &entry) {
        if (!transaction.transaction) {
            return;
        }
        if (entry.type != CatalogType::TABLE_ENTRY) {
            return;
        }
        // if we have transaction local insertions for this table - clear them
        auto &table_entry = entry.Cast<TableCatalogEntry>();
        auto &local_storage = LocalStorage::Get(transaction.transaction->Cast<GooseTransaction>());
        local_storage.DropTable(table_entry.GetStorage());
    }

    optional_ptr<CatalogEntry> GooseSchemaEntry::LookupEntry(CatalogTransaction transaction,
                                                             const EntryLookupInfo &lookup_info) {
        return GetCatalogSet(lookup_info.GetCatalogType()).GetEntry(transaction, lookup_info.GetEntryName());
    }

    CatalogSet::EntryLookup GooseSchemaEntry::LookupEntryDetailed(CatalogTransaction transaction,
                                                                  const EntryLookupInfo &lookup_info) {
        return GetCatalogSet(lookup_info.GetCatalogType()).GetEntryDetailed(transaction, lookup_info.GetEntryName());
    }

    SimilarCatalogEntry GooseSchemaEntry::GetSimilarEntry(CatalogTransaction transaction,
                                                          const EntryLookupInfo &lookup_info) {
        return GetCatalogSet(lookup_info.GetCatalogType()).SimilarEntry(transaction, lookup_info.GetEntryName());
    }

    CatalogSet &GooseSchemaEntry::GetCatalogSet(CatalogType type) {
        switch (type) {
            case CatalogType::VIEW_ENTRY:
            case CatalogType::TABLE_ENTRY:
                return tables;
            case CatalogType::INDEX_ENTRY:
                return indexes;
            case CatalogType::TABLE_FUNCTION_ENTRY:
            case CatalogType::TABLE_MACRO_ENTRY:
                return table_functions;
            case CatalogType::COPY_FUNCTION_ENTRY:
                return copy_functions;
            case CatalogType::PRAGMA_FUNCTION_ENTRY:
                return pragma_functions;
            case CatalogType::AGGREGATE_FUNCTION_ENTRY:
            case CatalogType::SCALAR_FUNCTION_ENTRY:
            case CatalogType::MACRO_ENTRY:
                return functions;
            case CatalogType::SEQUENCE_ENTRY:
                return sequences;
            case CatalogType::COLLATION_ENTRY:
                return collations;
            case CatalogType::TYPE_ENTRY:
                return types;
            default:
                throw InternalException({{"catalog_type", CatalogTypeToString(type)}},
                                        "Unsupported catalog type in schema");
        }
    }

    void GooseSchemaEntry::Verify(Catalog &catalog) {
        InCatalogEntry::Verify(catalog);

        tables.Verify(catalog);
        indexes.Verify(catalog);
        table_functions.Verify(catalog);
        copy_functions.Verify(catalog);
        pragma_functions.Verify(catalog);
        functions.Verify(catalog);
        sequences.Verify(catalog);
        collations.Verify(catalog);
        types.Verify(catalog);
    }
} // namespace goose
