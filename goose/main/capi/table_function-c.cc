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


#include <goose/catalog/catalog.h>
#include <goose/common/type_visitor.h>
#include <goose/common/types.h>
#include <goose/function/table_function.h>
#include <goose/main/capi/capi_internal.h>
#include <goose/main/capi/capi_internal_table.h>
#include <goose/main/client_context.h>
#include <goose/parser/parsed_data/create_table_function_info.h>
#include <goose/storage/statistics/node_statistics.h>

namespace goose {
    namespace {
        //===--------------------------------------------------------------------===//
        // Structures
        //===--------------------------------------------------------------------===//

        struct CTableInitData {
            ~CTableInitData() {
                if (init_data && delete_callback) {
                    delete_callback(init_data);
                }
                init_data = nullptr;
                delete_callback = nullptr;
            }

            void *init_data = nullptr;
            goose_delete_callback_t delete_callback = nullptr;
            idx_t max_threads = 1;
        };

        struct CTableGlobalInitData : public GlobalTableFunctionState {
            CTableInitData init_data;

            idx_t MaxThreads() const override {
                return init_data.max_threads;
            }
        };

        struct CTableLocalInitData : public LocalTableFunctionState {
            CTableInitData init_data;
        };

        struct CTableInternalInitInfo {
            CTableInternalInitInfo(const CTableBindData &bind_data, CTableInitData &init_data,
                                   const vector<column_t> &column_ids, optional_ptr<TableFilterSet> filters)
                : bind_data(bind_data), init_data(init_data), column_ids(column_ids), filters(filters), success(true) {
            }

            const CTableBindData &bind_data;
            CTableInitData &init_data;
            const vector<column_t> &column_ids;
            optional_ptr<TableFilterSet> filters;
            bool success;
            string error;
        };

        struct CTableInternalFunctionInfo {
            CTableInternalFunctionInfo(const CTableBindData &bind_data, CTableInitData &init_data,
                                       CTableInitData &local_data)
                : bind_data(bind_data), init_data(init_data), local_data(local_data), success(true) {
            }

            const CTableBindData &bind_data;
            CTableInitData &init_data;
            CTableInitData &local_data;
            bool success;
            string error;
        };

        //===--------------------------------------------------------------------===//
        // Helper Functions
        //===--------------------------------------------------------------------===//

        goose::TableFunction &GetCTableFunction(goose_table_function function) {
            return *reinterpret_cast<goose::TableFunction *>(function);
        }

        goose::CTableInternalBindInfo &GetCTableFunctionBindInfo(goose_bind_info info) {
            D_ASSERT(info);
            return *reinterpret_cast<goose::CTableInternalBindInfo *>(info);
        }

        goose_bind_info ToCTableFunctionBindInfo(goose::CTableInternalBindInfo &info) {
            return reinterpret_cast<goose_bind_info>(&info);
        }

        goose::CTableInternalInitInfo &GetCInitInfo(goose_init_info info) {
            D_ASSERT(info);
            return *reinterpret_cast<goose::CTableInternalInitInfo *>(info);
        }

        goose_init_info ToCInitInfo(goose::CTableInternalInitInfo &info) {
            return reinterpret_cast<goose_init_info>(&info);
        }

        goose::CTableInternalFunctionInfo &GetCTableFunctionInfo(goose_function_info info) {
            D_ASSERT(info);
            return *reinterpret_cast<goose::CTableInternalFunctionInfo *>(info);
        }

        goose_function_info ToCTableFunctionInfo(goose::CTableInternalFunctionInfo &info) {
            return reinterpret_cast<goose_function_info>(&info);
        }

        //===--------------------------------------------------------------------===//
        // Table Function Callbacks
        //===--------------------------------------------------------------------===//

        unique_ptr<FunctionData> CTableFunctionBind(ClientContext &context, TableFunctionBindInput &input,
                                                    vector<LogicalType> &return_types, vector<string> &names) {
            auto &info = input.info->Cast<CTableFunctionInfo>();
            D_ASSERT(info.bind && info.function && info.init);

            auto result = make_uniq<CTableBindData>(info);
            CTableInternalBindInfo bind_info(context, input.inputs, input.named_parameters, return_types, names,
                                             *result, info);
            info.bind(ToCTableFunctionBindInfo(bind_info));
            if (!bind_info.success) {
                throw BinderException(bind_info.error);
            }

            return std::move(result);
        }

        unique_ptr<GlobalTableFunctionState>
        CTableFunctionInit(ClientContext &context, TableFunctionInitInput &data_p) {
            auto &bind_data = data_p.bind_data->Cast<CTableBindData>();
            auto result = make_uniq<CTableGlobalInitData>();

            CTableInternalInitInfo init_info(bind_data, result->init_data, data_p.column_ids, data_p.filters);
            bind_data.info.init(ToCInitInfo(init_info));
            if (!init_info.success) {
                throw InvalidInputException(init_info.error);
            }
            return std::move(result);
        }

        unique_ptr<LocalTableFunctionState> CTableFunctionLocalInit(ExecutionContext &context,
                                                                    TableFunctionInitInput &data_p,
                                                                    GlobalTableFunctionState *gstate) {
            auto &bind_data = data_p.bind_data->Cast<CTableBindData>();
            auto result = make_uniq<CTableLocalInitData>();
            if (!bind_data.info.local_init) {
                return std::move(result);
            }

            CTableInternalInitInfo init_info(bind_data, result->init_data, data_p.column_ids, data_p.filters);
            bind_data.info.local_init(ToCInitInfo(init_info));
            if (!init_info.success) {
                throw InvalidInputException(init_info.error);
            }
            return std::move(result);
        }

        unique_ptr<NodeStatistics> CTableFunctionCardinality(ClientContext &context, const FunctionData *bind_data_p) {
            auto &bind_data = bind_data_p->Cast<CTableBindData>();
            if (!bind_data.stats) {
                return nullptr;
            }
            return make_uniq<NodeStatistics>(*bind_data.stats);
        }

        void CTableFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
            auto &bind_data = data_p.bind_data->Cast<CTableBindData>();
            auto &global_data = data_p.global_state->Cast<CTableGlobalInitData>();
            auto &local_data = data_p.local_state->Cast<CTableLocalInitData>();
            CTableInternalFunctionInfo function_info(bind_data, global_data.init_data, local_data.init_data);
            bind_data.info.function(ToCTableFunctionInfo(function_info), reinterpret_cast<goose_data_chunk>(&output));
            if (!function_info.success) {
                throw InvalidInputException(function_info.error);
            }
        }
    } // namespace
} // namespace goose

//===--------------------------------------------------------------------===//
// Table Function
//===--------------------------------------------------------------------===//
using goose::GetCTableFunction;

goose_table_function goose_create_table_function() {
    auto function = new goose::TableFunction("", {}, goose::CTableFunction, goose::CTableFunctionBind,
                                             goose::CTableFunctionInit, goose::CTableFunctionLocalInit);
    function->function_info = goose::make_shared_ptr<goose::CTableFunctionInfo>();
    function->cardinality = goose::CTableFunctionCardinality;
    return reinterpret_cast<goose_table_function>(function);
}

void goose_destroy_table_function(goose_table_function *function) {
    if (function && *function) {
        auto tf = reinterpret_cast<goose::TableFunction *>(*function);
        delete tf;
        *function = nullptr;
    }
}

void goose_table_function_set_name(goose_table_function function, const char *name) {
    if (!function || !name) {
        return;
    }
    auto &tf = GetCTableFunction(function);
    tf.name = name;
}

void goose_table_function_add_parameter(goose_table_function function, goose_logical_type type) {
    if (!function || !type) {
        return;
    }
    auto &tf = GetCTableFunction(function);
    auto logical_type = reinterpret_cast<goose::LogicalType *>(type);
    tf.arguments.push_back(*logical_type);
}

void goose_table_function_add_named_parameter(goose_table_function function, const char *name,
                                              goose_logical_type type) {
    if (!function || !type) {
        return;
    }
    auto &tf = GetCTableFunction(function);
    auto logical_type = reinterpret_cast<goose::LogicalType *>(type);
    tf.named_parameters.insert({name, *logical_type});
}

void goose_table_function_set_extra_info(goose_table_function function, void *extra_info,
                                         goose_delete_callback_t destroy) {
    if (!function) {
        return;
    }
    auto &tf = GetCTableFunction(function);
    auto &info = tf.function_info->Cast<goose::CTableFunctionInfo>();
    info.extra_info = extra_info;
    info.delete_callback = destroy;
}

void goose_table_function_set_bind(goose_table_function function, goose_table_function_bind_t bind) {
    if (!function || !bind) {
        return;
    }
    auto &tf = GetCTableFunction(function);
    auto &info = tf.function_info->Cast<goose::CTableFunctionInfo>();
    info.bind = bind;
}

void goose_table_function_set_init(goose_table_function function, goose_table_function_init_t init) {
    if (!function || !init) {
        return;
    }
    auto &tf = GetCTableFunction(function);
    auto &info = tf.function_info->Cast<goose::CTableFunctionInfo>();
    info.init = init;
}

void goose_table_function_set_local_init(goose_table_function function, goose_table_function_init_t init) {
    if (!function || !init) {
        return;
    }
    auto &tf = GetCTableFunction(function);
    auto &info = tf.function_info->Cast<goose::CTableFunctionInfo>();
    info.local_init = init;
}

void goose_table_function_set_function(goose_table_function table_function, goose_table_function_t function) {
    if (!table_function || !function) {
        return;
    }
    auto &tf = GetCTableFunction(table_function);
    auto &info = tf.function_info->Cast<goose::CTableFunctionInfo>();
    info.function = function;
}

void goose_table_function_supports_projection_pushdown(goose_table_function table_function, bool pushdown) {
    if (!table_function) {
        return;
    }
    auto &tf = GetCTableFunction(table_function);
    tf.projection_pushdown = pushdown;
}

goose_state goose_register_table_function(goose_connection connection, goose_table_function function) {
    if (!connection || !function) {
        return GooseError;
    }
    auto con = reinterpret_cast<goose::Connection *>(connection);
    auto &tf = GetCTableFunction(function);
    auto &info = tf.function_info->Cast<goose::CTableFunctionInfo>();

    if (tf.name.empty() || !info.bind || !info.init || !info.function) {
        return GooseError;
    }
    for (auto it = tf.named_parameters.begin(); it != tf.named_parameters.end(); it++) {
        if (goose::TypeVisitor::Contains(it->second, goose::LogicalTypeId::INVALID)) {
            return GooseError;
        }
    }
    for (const auto &argument: tf.arguments) {
        if (goose::TypeVisitor::Contains(argument, goose::LogicalTypeId::INVALID)) {
            return GooseError;
        }
    }

    try {
        con->context->RunFunctionInTransaction([&]() {
            auto &catalog = goose::Catalog::GetSystemCatalog(*con->context);
            goose::CreateTableFunctionInfo tf_info(tf);
            tf_info.on_conflict = goose::OnCreateConflict::ALTER_ON_CONFLICT;
            catalog.CreateTableFunction(*con->context, tf_info);
        });
    } catch (...) {
        // LCOV_EXCL_START
        return GooseError;
    } // LCOV_EXCL_STOP
    return GooseSuccess;
}

//===--------------------------------------------------------------------===//
// Bind Interface
//===--------------------------------------------------------------------===//
using goose::GetCTableFunctionBindInfo;

void *goose_bind_get_extra_info(goose_bind_info info) {
    if (!info) {
        return nullptr;
    }
    auto &bind_info = GetCTableFunctionBindInfo(info);
    return bind_info.function_info.extra_info;
}

void goose_table_function_get_client_context(goose_bind_info info, goose_client_context *out_context) {
    if (!info || !out_context) {
        return;
    }
    auto &bind_info = GetCTableFunctionBindInfo(info);
    auto wrapper = new goose::CClientContextWrapper(bind_info.context);
    *out_context = reinterpret_cast<goose_client_context>(wrapper);
}

void goose_bind_add_result_column(goose_bind_info info, const char *name, goose_logical_type type) {
    if (!info || !name || !type) {
        return;
    }
    auto logical_type = reinterpret_cast<goose::LogicalType *>(type);
    if (goose::TypeVisitor::Contains(*logical_type, goose::LogicalTypeId::INVALID) ||
        goose::TypeVisitor::Contains(*logical_type, goose::LogicalTypeId::ANY)) {
        return;
    }

    auto &bind_info = GetCTableFunctionBindInfo(info);
    bind_info.names.push_back(name);
    bind_info.return_types.push_back(*logical_type);
}

idx_t goose_bind_get_parameter_count(goose_bind_info info) {
    if (!info) {
        return 0;
    }
    auto &bind_info = GetCTableFunctionBindInfo(info);
    return bind_info.parameters.size();
}

goose_value goose_bind_get_parameter(goose_bind_info info, idx_t index) {
    if (!info || index >= goose_bind_get_parameter_count(info)) {
        return nullptr;
    }
    auto &bind_info = GetCTableFunctionBindInfo(info);
    return reinterpret_cast<goose_value>(new goose::Value(bind_info.parameters[index]));
}

goose_value goose_bind_get_named_parameter(goose_bind_info info, const char *name) {
    if (!info || !name) {
        return nullptr;
    }
    auto &bind_info = GetCTableFunctionBindInfo(info);
    auto t = bind_info.named_parameters.find(name);
    if (t == bind_info.named_parameters.end()) {
        return nullptr;
    } else {
        return reinterpret_cast<goose_value>(new goose::Value(t->second));
    }
}

void goose_bind_set_bind_data(goose_bind_info info, void *bind_data, goose_delete_callback_t destroy) {
    if (!info) {
        return;
    }
    auto &bind_info = GetCTableFunctionBindInfo(info);
    bind_info.bind_data.bind_data = bind_data;
    bind_info.bind_data.delete_callback = destroy;
}

void goose_bind_set_cardinality(goose_bind_info info, idx_t cardinality, bool is_exact) {
    if (!info) {
        return;
    }
    auto &bind_info = GetCTableFunctionBindInfo(info);
    if (is_exact) {
        bind_info.bind_data.stats = goose::make_uniq<goose::NodeStatistics>(cardinality);
    } else {
        bind_info.bind_data.stats = goose::make_uniq<goose::NodeStatistics>(cardinality, cardinality);
    }
}

void goose_bind_set_error(goose_bind_info info, const char *error) {
    if (!info || !error) {
        return;
    }
    auto &bind_info = GetCTableFunctionBindInfo(info);
    bind_info.error = error;
    bind_info.success = false;
}

//===--------------------------------------------------------------------===//
// Init Interface
//===--------------------------------------------------------------------===//
using goose::GetCInitInfo;

void *goose_init_get_extra_info(goose_init_info info) {
    if (!info) {
        return nullptr;
    }
    auto init_info = reinterpret_cast<goose::CTableInternalInitInfo *>(info);
    return init_info->bind_data.info.extra_info;
}

void *goose_init_get_bind_data(goose_init_info info) {
    if (!info) {
        return nullptr;
    }
    auto &init_info = GetCInitInfo(info);
    return init_info.bind_data.bind_data;
}

void goose_init_set_init_data(goose_init_info info, void *init_data, goose_delete_callback_t destroy) {
    if (!info) {
        return;
    }
    auto &init_info = GetCInitInfo(info);
    init_info.init_data.init_data = init_data;
    init_info.init_data.delete_callback = destroy;
}

void goose_init_set_error(goose_init_info info, const char *error) {
    if (!info || !error) {
        return;
    }
    auto &init_info = GetCInitInfo(info);
    init_info.error = error;
    init_info.success = false;
}

idx_t goose_init_get_column_count(goose_init_info info) {
    if (!info) {
        return 0;
    }
    auto &init_info = GetCInitInfo(info);
    return init_info.column_ids.size();
}

idx_t goose_init_get_column_index(goose_init_info info, idx_t column_index) {
    if (!info) {
        return 0;
    }
    auto &init_info = GetCInitInfo(info);
    if (column_index >= init_info.column_ids.size()) {
        return 0;
    }
    return init_info.column_ids[column_index];
}

void goose_init_set_max_threads(goose_init_info info, idx_t max_threads) {
    if (!info) {
        return;
    }
    auto &init_info = GetCInitInfo(info);
    init_info.init_data.max_threads = max_threads;
}

//===--------------------------------------------------------------------===//
// Function Interface
//===--------------------------------------------------------------------===//
using goose::GetCTableFunctionInfo;

void *goose_function_get_extra_info(goose_function_info info) {
    if (!info) {
        return nullptr;
    }
    auto &function_info = GetCTableFunctionInfo(info);
    return function_info.bind_data.info.extra_info;
}

void *goose_function_get_bind_data(goose_function_info info) {
    if (!info) {
        return nullptr;
    }
    auto &function_info = GetCTableFunctionInfo(info);
    return function_info.bind_data.bind_data;
}

void *goose_function_get_init_data(goose_function_info info) {
    if (!info) {
        return nullptr;
    }
    auto &function_info = GetCTableFunctionInfo(info);
    return function_info.init_data.init_data;
}

void *goose_function_get_local_init_data(goose_function_info info) {
    if (!info) {
        return nullptr;
    }
    auto &function_info = GetCTableFunctionInfo(info);
    return function_info.local_data.init_data;
}

void goose_function_set_error(goose_function_info info, const char *error) {
    if (!info || !error) {
        return;
    }
    auto &function_info = GetCTableFunctionInfo(info);
    function_info.error = error;
    function_info.success = false;
}
