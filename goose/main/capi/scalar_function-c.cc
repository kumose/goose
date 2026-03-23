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
#include <goose/function/function.h>
#include <goose/function/scalar_function.h>
#include <goose/main/capi/capi_internal.h>
#include <goose/main/client_context.h>
#include <goose/parser/parsed_data/create_scalar_function_info.h>
#include <goose/planner/expression/bound_function_expression.h>

namespace goose {
    namespace {
        struct CScalarFunctionInfo : public ScalarFunctionInfo {
            ~CScalarFunctionInfo() override {
                if (extra_info && delete_callback) {
                    delete_callback(extra_info);
                }
                extra_info = nullptr;
                delete_callback = nullptr;
            }

            goose_scalar_function_bind_t bind = nullptr;
            goose_scalar_function_init_t init = nullptr;
            goose_scalar_function_t function = nullptr;
            goose_function_info extra_info = nullptr;
            goose_delete_callback_t delete_callback = nullptr;
        };

        struct CScalarFunctionBindData : public FunctionData {
            explicit CScalarFunctionBindData(CScalarFunctionInfo &info) : info(info) {
            }

            ~CScalarFunctionBindData() override {
                if (bind_data && delete_callback) {
                    delete_callback(bind_data);
                }
                bind_data = nullptr;
                delete_callback = nullptr;
            }

            unique_ptr<FunctionData> Copy() const override {
                auto copy = make_uniq<CScalarFunctionBindData>(info);
                if (copy_callback) {
                    copy->bind_data = copy_callback(bind_data);
                    copy->delete_callback = delete_callback;
                    copy->copy_callback = copy_callback;
                }
                return copy;
            }

            bool Equals(const FunctionData &other_p) const override {
                auto &other = other_p.Cast<CScalarFunctionBindData>();
                return info.extra_info == other.info.extra_info && info.function == other.info.function;
            }

            CScalarFunctionInfo &info;
            void *bind_data = nullptr;
            goose_delete_callback_t delete_callback = nullptr;
            goose_copy_callback_t copy_callback = nullptr;
        };

        struct CScalarFunctionInternalBindInfo {
            CScalarFunctionInternalBindInfo(ClientContext &context, ScalarFunction &bound_function,
                                            vector<unique_ptr<Expression> > &arguments,
                                            CScalarFunctionBindData &bind_data)
                : context(context), bound_function(bound_function), arguments(arguments), bind_data(bind_data) {
            }

            ClientContext &context;
            ScalarFunction &bound_function;
            vector<unique_ptr<Expression> > &arguments;
            CScalarFunctionBindData &bind_data;

            bool success = true;
            string error = "";
        };

        struct CScalarFunctionLocalState : public FunctionLocalState {
            ~CScalarFunctionLocalState() override {
                if (local_state && delete_callback) {
                    delete_callback(local_state);
                }
                local_state = nullptr;
                delete_callback = nullptr;
            }

            void *local_state;
            goose_delete_callback_t delete_callback = nullptr;
        };

        struct CScalarFunctionInternalInitInfo {
            CScalarFunctionInternalInitInfo(ClientContext &context, CScalarFunctionBindData &bind_data,
                                            CScalarFunctionLocalState &local_state)
                : context(context), bind_data(bind_data), local_state(local_state) {
            }

            ClientContext &context;
            CScalarFunctionBindData &bind_data;
            CScalarFunctionLocalState &local_state;

            bool success = true;
            string error = "";
        };

        struct CScalarFunctionInternalFunctionInfo {
            explicit CScalarFunctionInternalFunctionInfo(const CScalarFunctionBindData &bind_data,
                                                         CScalarFunctionLocalState &local_state)
                : bind_data(bind_data), local_state(local_state), success(true) {
            };

            const CScalarFunctionBindData &bind_data;
            CScalarFunctionLocalState &local_state;

            bool success;
            string error = "";
        };

        //===--------------------------------------------------------------------===//
        // Helper Functions
        //===--------------------------------------------------------------------===//

        goose::ScalarFunction &GetCScalarFunction(goose_scalar_function function) {
            return *reinterpret_cast<goose::ScalarFunction *>(function);
        }

        goose::ScalarFunctionSet &GetCScalarFunctionSet(goose_scalar_function_set set) {
            return *reinterpret_cast<goose::ScalarFunctionSet *>(set);
        }

        goose::CScalarFunctionInternalBindInfo &GetCScalarFunctionBindInfo(goose_bind_info info) {
            D_ASSERT(info);
            return *reinterpret_cast<goose::CScalarFunctionInternalBindInfo *>(info);
        }

        goose_bind_info ToCScalarFunctionBindInfo(goose::CScalarFunctionInternalBindInfo &info) {
            return reinterpret_cast<goose_bind_info>(&info);
        }

        goose::CScalarFunctionInternalInitInfo &GetCScalarFunctionInitInfo(goose_init_info info) {
            D_ASSERT(info);
            return *reinterpret_cast<goose::CScalarFunctionInternalInitInfo *>(info);
        }

        goose_init_info ToCScalarFunctionInitInfo(goose::CScalarFunctionInternalInitInfo &info) {
            return reinterpret_cast<goose_init_info>(&info);
        }

        goose::CScalarFunctionInternalFunctionInfo &GetCScalarFunctionInfo(goose_function_info info) {
            D_ASSERT(info);
            return *reinterpret_cast<goose::CScalarFunctionInternalFunctionInfo *>(info);
        }

        goose_function_info ToCScalarFunctionInfo(goose::CScalarFunctionInternalFunctionInfo &info) {
            return reinterpret_cast<goose_function_info>(&info);
        }

        //===--------------------------------------------------------------------===//
        // Scalar Function Callbacks
        //===--------------------------------------------------------------------===//

        unique_ptr<FunctionData> CScalarFunctionBind(ClientContext &context, ScalarFunction &bound_function,
                                                     vector<unique_ptr<Expression> > &arguments) {
            auto &info = bound_function.GetExtraFunctionInfo().Cast<CScalarFunctionInfo>();
            D_ASSERT(info.function);

            auto result = make_uniq<CScalarFunctionBindData>(info);
            if (info.bind) {
                CScalarFunctionInternalBindInfo bind_info(context, bound_function, arguments, *result);
                info.bind(ToCScalarFunctionBindInfo(bind_info));
                if (!bind_info.success) {
                    throw BinderException(bind_info.error);
                }
            }

            return result;
        }

        unique_ptr<FunctionLocalState> CScalarFunctionInit(ExpressionState &state, const BoundFunctionExpression &expr,
                                                           FunctionData *bind_data) {
            auto &function = expr.function;
            auto &info = function.GetExtraFunctionInfo().Cast<CScalarFunctionInfo>();
            D_ASSERT(info.function);

            auto result = make_uniq<CScalarFunctionLocalState>();
            if (info.init) {
                auto &context = state.GetContext();
                auto &c_bind_data = bind_data->Cast<CScalarFunctionBindData>();
                CScalarFunctionInternalInitInfo init_info(context, c_bind_data, *result);
                info.init(ToCScalarFunctionInitInfo(init_info));
                if (!init_info.success) {
                    throw InvalidInputException(init_info.error);
                }
            }
            return result;
        }

        void CAPIScalarFunction(DataChunk &input, ExpressionState &state, Vector &result) {
            auto &function = state.expr.Cast<BoundFunctionExpression>();
            auto &bind_info = function.bind_info;
            auto &c_bind_info = bind_info->Cast<CScalarFunctionBindData>();
            auto &c_local_state = ExecuteFunctionState::GetFunctionState(state)->Cast<CScalarFunctionLocalState>();

            auto all_const = input.AllConstant();
            input.Flatten();
            auto c_input = reinterpret_cast<goose_data_chunk>(&input);
            auto c_result = reinterpret_cast<goose_vector>(&result);

            CScalarFunctionInternalFunctionInfo function_info(c_bind_info, c_local_state);
            auto c_function_info = ToCScalarFunctionInfo(function_info);
            c_bind_info.info.function(c_function_info, c_input, c_result);
            if (!function_info.success) {
                throw InvalidInputException(function_info.error);
            }
            if (all_const && (input.size() == 1 || function.function.GetStability() != FunctionStability::VOLATILE)) {
                result.SetVectorType(VectorType::CONSTANT_VECTOR);
            }
        }
    } // namespace
} // namespace goose

using goose::ExpressionWrapper;
using goose::GetCScalarFunction;
using goose::GetCScalarFunctionBindInfo;
using goose::GetCScalarFunctionInfo;
using goose::GetCScalarFunctionSet;

goose_scalar_function goose_create_scalar_function() {
    auto function = new goose::ScalarFunction("", {}, goose::LogicalType::INVALID, goose::CAPIScalarFunction,
                                              goose::CScalarFunctionBind);
    function->init_local_state = goose::CScalarFunctionInit;
    function->SetExtraFunctionInfo<goose::CScalarFunctionInfo>();
    return reinterpret_cast<goose_scalar_function>(function);
}

void goose_destroy_scalar_function(goose_scalar_function *function) {
    if (function && *function) {
        auto scalar_function = reinterpret_cast<goose::ScalarFunction *>(*function);
        delete scalar_function;
        *function = nullptr;
    }
}

void goose_scalar_function_set_name(goose_scalar_function function, const char *name) {
    if (!function || !name) {
        return;
    }
    auto &scalar_function = GetCScalarFunction(function);
    scalar_function.name = name;
}

void goose_scalar_function_set_varargs(goose_scalar_function function, goose_logical_type type) {
    if (!function || !type) {
        return;
    }
    auto &scalar_function = GetCScalarFunction(function);
    auto logical_type = reinterpret_cast<goose::LogicalType *>(type);
    scalar_function.varargs = *logical_type;
}

void goose_scalar_function_set_special_handling(goose_scalar_function function) {
    if (!function) {
        return;
    }
    auto &scalar_function = GetCScalarFunction(function);
    scalar_function.SetNullHandling(goose::FunctionNullHandling::SPECIAL_HANDLING);
}

void goose_scalar_function_set_volatile(goose_scalar_function function) {
    if (!function) {
        return;
    }
    auto &scalar_function = GetCScalarFunction(function);
    scalar_function.SetVolatile();
}

void goose_scalar_function_add_parameter(goose_scalar_function function, goose_logical_type type) {
    if (!function || !type) {
        return;
    }
    auto &scalar_function = GetCScalarFunction(function);
    auto logical_type = reinterpret_cast<goose::LogicalType *>(type);
    scalar_function.arguments.push_back(*logical_type);
}

void goose_scalar_function_set_return_type(goose_scalar_function function, goose_logical_type type) {
    if (!function || !type) {
        return;
    }
    auto &scalar_function = GetCScalarFunction(function);
    auto logical_type = reinterpret_cast<goose::LogicalType *>(type);
    scalar_function.SetReturnType(*logical_type);
}

void *goose_scalar_function_get_extra_info(goose_function_info info) {
    if (!info) {
        return nullptr;
    }
    auto &function_info = GetCScalarFunctionInfo(info);
    return function_info.bind_data.info.extra_info;
}

void *goose_scalar_function_bind_get_extra_info(goose_bind_info info) {
    if (!info) {
        return nullptr;
    }
    auto &bind_info = GetCScalarFunctionBindInfo(info);
    return bind_info.bind_data.info.extra_info;
}

void *goose_scalar_function_get_bind_data(goose_function_info info) {
    if (!info) {
        return nullptr;
    }
    auto &function_info = GetCScalarFunctionInfo(info);
    return function_info.bind_data.bind_data;
}

void goose_scalar_function_get_client_context(goose_bind_info info, goose_client_context *out_context) {
    if (!info || !out_context) {
        return;
    }
    auto &bind_info = GetCScalarFunctionBindInfo(info);
    auto wrapper = new goose::CClientContextWrapper(bind_info.context);
    *out_context = reinterpret_cast<goose_client_context>(wrapper);
}

void goose_scalar_function_set_error(goose_function_info info, const char *error) {
    if (!info || !error) {
        return;
    }
    auto &scalar_function = goose::GetCScalarFunctionInfo(info);
    scalar_function.error = error;
    scalar_function.success = false;
}

void goose_scalar_function_bind_set_error(goose_bind_info info, const char *error) {
    if (!info || !error) {
        return;
    }
    auto &bind_info = GetCScalarFunctionBindInfo(info);
    bind_info.error = error;
    bind_info.success = false;
}

idx_t goose_scalar_function_bind_get_argument_count(goose_bind_info info) {
    if (!info) {
        return 0;
    }
    auto &bind_info = GetCScalarFunctionBindInfo(info);
    return bind_info.arguments.size();
}

goose_expression goose_scalar_function_bind_get_argument(goose_bind_info info, idx_t index) {
    if (!info || index >= goose_scalar_function_bind_get_argument_count(info)) {
        return nullptr;
    }
    auto &bind_info = GetCScalarFunctionBindInfo(info);
    auto wrapper = new ExpressionWrapper();
    wrapper->expr = bind_info.arguments[index]->Copy();
    return reinterpret_cast<goose_expression>(wrapper);
}

void goose_scalar_function_set_extra_info(goose_scalar_function function, void *extra_info,
                                          goose_delete_callback_t destroy) {
    if (!function || !extra_info) {
        return;
    }
    auto &scalar_function = GetCScalarFunction(function);
    auto &info = scalar_function.GetExtraFunctionInfo().Cast<goose::CScalarFunctionInfo>();
    info.extra_info = reinterpret_cast<goose_function_info>(extra_info);
    info.delete_callback = destroy;
}

void goose_scalar_function_set_bind(goose_scalar_function scalar_function, goose_scalar_function_bind_t bind) {
    if (!scalar_function || !bind) {
        return;
    }
    auto &sf = GetCScalarFunction(scalar_function);
    auto &info = sf.GetExtraFunctionInfo().Cast<goose::CScalarFunctionInfo>();
    info.bind = bind;
}

void goose_scalar_function_set_bind_data(goose_bind_info info, void *bind_data, goose_delete_callback_t destroy) {
    if (!info) {
        return;
    }
    auto &bind_info = GetCScalarFunctionBindInfo(info);
    bind_info.bind_data.bind_data = bind_data;
    bind_info.bind_data.delete_callback = destroy;
}

void goose_scalar_function_set_bind_data_copy(goose_bind_info info, goose_copy_callback_t copy) {
    if (!info) {
        return;
    }
    auto &bind_info = GetCScalarFunctionBindInfo(info);
    bind_info.bind_data.copy_callback = copy;
}

void goose_scalar_function_set_function(goose_scalar_function function, goose_scalar_function_t execute_func) {
    if (!function || !execute_func) {
        return;
    }
    auto &scalar_function = GetCScalarFunction(function);
    auto &info = scalar_function.GetExtraFunctionInfo().Cast<goose::CScalarFunctionInfo>();
    info.function = execute_func;
}

void *goose_scalar_function_get_state(goose_function_info info) {
    if (!info) {
        return nullptr;
    }
    auto &function_info = GetCScalarFunctionInfo(info);
    return function_info.local_state.local_state;
}

void goose_scalar_function_set_init(goose_scalar_function function, goose_scalar_function_init_t init_func) {
    if (!function || !init_func) {
        return;
    }
    auto &scalar_function = GetCScalarFunction(function);
    auto &info = scalar_function.GetExtraFunctionInfo().Cast<goose::CScalarFunctionInfo>();
    info.init = init_func;
}

void goose_scalar_function_init_set_error(goose_init_info info, const char *error) {
    if (!info || !error) {
        return;
    }
    auto &scalar_function = goose::GetCScalarFunctionInitInfo(info);
    scalar_function.error = error;
    scalar_function.success = false;
}

void goose_scalar_function_init_set_state(goose_init_info info, void *local_state, goose_delete_callback_t destroy) {
    if (!info) {
        return;
    }
    auto &init_info = goose::GetCScalarFunctionInitInfo(info);
    init_info.local_state.local_state = local_state;
    init_info.local_state.delete_callback = destroy;
}

void goose_scalar_function_init_get_client_context(goose_init_info info, goose_client_context *out_context) {
    if (!info || !out_context) {
        return;
    }
    auto &init_info = goose::GetCScalarFunctionInitInfo(info);
    auto &context = init_info.context;
    auto wrapper = new goose::CClientContextWrapper(context);
    *out_context = reinterpret_cast<goose_client_context>(wrapper);
}

void *goose_scalar_function_init_get_bind_data(goose_init_info info) {
    if (!info) {
        return nullptr;
    }
    auto &init_info = goose::GetCScalarFunctionInitInfo(info);
    return init_info.bind_data.bind_data;
}

void *goose_scalar_function_init_get_extra_info(goose_init_info info) {
    if (!info) {
        return nullptr;
    }
    auto &init_info = goose::GetCScalarFunctionInitInfo(info);
    return init_info.bind_data.info.extra_info;
}

goose_state goose_register_scalar_function(goose_connection connection, goose_scalar_function function) {
    if (!connection || !function) {
        return GooseError;
    }
    auto &scalar_function = GetCScalarFunction(function);
    goose::ScalarFunctionSet set(scalar_function.name);
    set.AddFunction(scalar_function);
    return goose_register_scalar_function_set(connection, reinterpret_cast<goose_scalar_function_set>(&set));
}

goose_scalar_function_set goose_create_scalar_function_set(const char *name) {
    if (!name || !*name) {
        return nullptr;
    }
    auto function = new goose::ScalarFunctionSet(name);
    return reinterpret_cast<goose_scalar_function_set>(function);
}

void goose_destroy_scalar_function_set(goose_scalar_function_set *set) {
    if (set && *set) {
        auto scalar_function_set = reinterpret_cast<goose::ScalarFunctionSet *>(*set);
        delete scalar_function_set;
        *set = nullptr;
    }
}

goose_state goose_add_scalar_function_to_set(goose_scalar_function_set set, goose_scalar_function function) {
    if (!set || !function) {
        return GooseError;
    }
    auto &scalar_function_set = GetCScalarFunctionSet(set);
    auto &scalar_function = GetCScalarFunction(function);
    scalar_function_set.AddFunction(scalar_function);
    return GooseSuccess;
}

goose_state goose_register_scalar_function_set(goose_connection connection, goose_scalar_function_set set) {
    if (!connection || !set) {
        return GooseError;
    }
    auto &scalar_function_set = GetCScalarFunctionSet(set);
    for (idx_t idx = 0; idx < scalar_function_set.Size(); idx++) {
        auto &scalar_function = scalar_function_set.GetFunctionReferenceByOffset(idx);
        auto &info = scalar_function.GetExtraFunctionInfo().Cast<goose::CScalarFunctionInfo>();

        if (scalar_function.name.empty() || !info.function) {
            return GooseError;
        }
        if (goose::TypeVisitor::Contains(scalar_function.GetReturnType(), goose::LogicalTypeId::INVALID) ||
            goose::TypeVisitor::Contains(scalar_function.GetReturnType(), goose::LogicalTypeId::ANY)) {
            return GooseError;
        }
        for (const auto &argument: scalar_function.arguments) {
            if (goose::TypeVisitor::Contains(argument, goose::LogicalTypeId::INVALID)) {
                return GooseError;
            }
        }
    }

    try {
        auto con = reinterpret_cast<goose::Connection *>(connection);
        con->context->RunFunctionInTransaction([&]() {
            auto &catalog = goose::Catalog::GetSystemCatalog(*con->context);
            goose::CreateScalarFunctionInfo sf_info(scalar_function_set);
            sf_info.on_conflict = goose::OnCreateConflict::ALTER_ON_CONFLICT;
            catalog.CreateFunction(*con->context, sf_info);
        });
    } catch (...) {
        return GooseError;
    }
    return GooseSuccess;
}
