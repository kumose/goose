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


#include <goose/parser/statement/drop_statement.h>
#include <goose/main/secret/secret_manager.h>
#include <goose/parser/transformer.h>

namespace goose {
    unique_ptr<SQLStatement> Transformer::TransformDrop(cantor::PGDropStmt &stmt) {
        auto result = make_uniq<DropStatement>();
        auto &info = *result->info.get();
        if (stmt.objects->length != 1) {
            throw NotImplementedException("Can only drop one object at a time");
        }
        switch (stmt.removeType) {
            case cantor::PG_OBJECT_TABLE:
                info.type = CatalogType::TABLE_ENTRY;
                break;
            case cantor::PG_OBJECT_SCHEMA:
                info.type = CatalogType::SCHEMA_ENTRY;
                break;
            case cantor::PG_OBJECT_INDEX:
                info.type = CatalogType::INDEX_ENTRY;
                break;
            case cantor::PG_OBJECT_VIEW:
                info.type = CatalogType::VIEW_ENTRY;
                break;
            case cantor::PG_OBJECT_SEQUENCE:
                info.type = CatalogType::SEQUENCE_ENTRY;
                break;
            case cantor::PG_OBJECT_FUNCTION:
                info.type = CatalogType::MACRO_ENTRY;
                break;
            case cantor::PG_OBJECT_TABLE_MACRO:
                info.type = CatalogType::TABLE_MACRO_ENTRY;
                break;
            case cantor::PG_OBJECT_TYPE:
                info.type = CatalogType::TYPE_ENTRY;
                break;
            default:
                throw NotImplementedException("Cannot drop this type yet");
        }

        switch (stmt.removeType) {
            case cantor::PG_OBJECT_SCHEMA: {
                auto view_list = PGPointerCast<cantor::PGList>(stmt.objects->head->data.ptr_value);
                if (view_list->length == 2) {
                    info.catalog = PGPointerCast<cantor::PGValue>(view_list->head->data.ptr_value)->val.str;
                    info.name = PGPointerCast<cantor::PGValue>(view_list->head->next->data.ptr_value)->val.str;
                } else if (view_list->length == 1) {
                    info.name = PGPointerCast<cantor::PGValue>(view_list->head->data.ptr_value)->val.str;
                } else {
                    throw ParserException("Expected \"catalog.schema\" or \"schema\"");
                }
                break;
            }
            default: {
                auto view_list = PGPointerCast<cantor::PGList>(stmt.objects->head->data.ptr_value);
                if (view_list->length == 3) {
                    info.catalog = PGPointerCast<cantor::PGValue>(view_list->head->data.ptr_value)->val.str;
                    info.schema = PGPointerCast<cantor::PGValue>(view_list->head->next->data.ptr_value)->val.str;
                    info.name = PGPointerCast<cantor::PGValue>(view_list->head->next->next->data.ptr_value)->val.str;
                } else if (view_list->length == 2) {
                    info.schema = PGPointerCast<cantor::PGValue>(view_list->head->data.ptr_value)->val.str;
                    info.name = PGPointerCast<cantor::PGValue>(view_list->head->next->data.ptr_value)->val.str;
                } else if (view_list->length == 1) {
                    info.name = PGPointerCast<cantor::PGValue>(view_list->head->data.ptr_value)->val.str;
                } else {
                    throw ParserException("Expected \"catalog.schema.name\", \"schema.name\"or \"name\"");
                }
                break;
            }
        }
        info.cascade = stmt.behavior == cantor::PGDropBehavior::PG_DROP_CASCADE;
        info.if_not_found = TransformOnEntryNotFound(stmt.missing_ok);
        return result;
    }

    unique_ptr<DropStatement> Transformer::TransformDropSecret(cantor::PGDropSecretStmt &stmt) {
        auto result = make_uniq<DropStatement>();
        auto info = make_uniq<DropInfo>();
        auto extra_info = make_uniq<ExtraDropSecretInfo>();

        info->type = CatalogType::SECRET_ENTRY;
        info->name = stmt.secret_name;
        info->if_not_found = stmt.missing_ok ? OnEntryNotFound::RETURN_NULL : OnEntryNotFound::THROW_EXCEPTION;

        extra_info->persist_mode = EnumUtil::FromString<SecretPersistType>(StringUtil::Upper(stmt.persist_type));
        extra_info->secret_storage = stmt.secret_storage;

        if (extra_info->persist_mode == SecretPersistType::TEMPORARY) {
            if (!extra_info->secret_storage.empty()) {
                throw ParserException("Can not combine TEMPORARY with specifying a storage for drop secret");
            }
        }

        info->extra_drop_info = std::move(extra_info);
        result->info = std::move(info);

        return result;
    }
} // namespace goose
