#include <goose/function/table/system_functions.h>

#include <goose/catalog/catalog.h>
#include <goose/catalog/goose_catalog.h>
#include <goose/catalog/dependency_manager.h>
#include <goose/common/exception.h>
#include <goose/main/client_context.h>

namespace goose {
    struct DependencyInformation {
        DependencyInformation(CatalogEntry &object, CatalogEntry &dependent, const DependencyDependentFlags &flags)
            : object(object), dependent(dependent), flags(flags) {
        }

        CatalogEntry &object;
        CatalogEntry &dependent;
        DependencyDependentFlags flags;
    };

    struct GooseDependenciesData : public GlobalTableFunctionState {
        GooseDependenciesData() : offset(0) {
        }

        vector<DependencyInformation> entries;
        idx_t offset;
    };

    static unique_ptr<FunctionData> GooseDependenciesBind(ClientContext &context, TableFunctionBindInput &input,
                                                          vector<LogicalType> &return_types, vector<string> &names) {
        names.emplace_back("classid");
        return_types.emplace_back(LogicalType::BIGINT);

        names.emplace_back("objid");
        return_types.emplace_back(LogicalType::BIGINT);

        names.emplace_back("objsubid");
        return_types.emplace_back(LogicalType::INTEGER);

        names.emplace_back("refclassid");
        return_types.emplace_back(LogicalType::BIGINT);

        names.emplace_back("refobjid");
        return_types.emplace_back(LogicalType::BIGINT);

        names.emplace_back("refobjsubid");
        return_types.emplace_back(LogicalType::INTEGER);

        names.emplace_back("deptype");
        return_types.emplace_back(LogicalType::VARCHAR);

        return nullptr;
    }

    unique_ptr<GlobalTableFunctionState> GooseDependenciesInit(ClientContext &context, TableFunctionInitInput &input) {
        auto result = make_uniq<GooseDependenciesData>();

        // scan all the schemas and collect them
        auto &catalog = Catalog::GetCatalog(context, INVALID_CATALOG);
        auto dependency_manager = catalog.GetDependencyManager();
        if (dependency_manager) {
            dependency_manager->Scan(
                context, [&](CatalogEntry &obj, CatalogEntry &dependent, const DependencyDependentFlags &flags) {
                    result->entries.emplace_back(obj, dependent, flags);
                });
        }

        return std::move(result);
    }

    void GooseDependenciesFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
        auto &data = data_p.global_state->Cast<GooseDependenciesData>();
        if (data.offset >= data.entries.size()) {
            // finished returning values
            return;
        }
        // start returning values
        // either fill up the chunk or return all the remaining columns
        idx_t count = 0;
        while (data.offset < data.entries.size() && count < STANDARD_VECTOR_SIZE) {
            auto &entry = data.entries[data.offset];

            // return values:
            // classid, LogicalType::BIGINT
            output.SetValue(0, count, Value::BIGINT(0));
            // objid, LogicalType::BIGINT
            output.SetValue(1, count, Value::BIGINT(NumericCast<int64_t>(entry.object.oid)));
            // objsubid, LogicalType::INTEGER
            output.SetValue(2, count, Value::INTEGER(0));
            // refclassid, LogicalType::BIGINT
            output.SetValue(3, count, Value::BIGINT(0));
            // refobjid, LogicalType::BIGINT
            output.SetValue(4, count, Value::BIGINT(NumericCast<int64_t>(entry.dependent.oid)));
            // refobjsubid, LogicalType::INTEGER
            output.SetValue(5, count, Value::INTEGER(0));
            // deptype, LogicalType::VARCHAR
            string dependency_type_str;
            if (entry.flags.IsBlocking()) {
                dependency_type_str = "n";
            } else {
                dependency_type_str = "a";
            }
            output.SetValue(6, count, Value(dependency_type_str));

            data.offset++;
            count++;
        }
        output.SetCardinality(count);
    }

    void GooseDependenciesFun::RegisterFunction(BuiltinFunctions &set) {
        set.AddFunction(TableFunction("goose_dependencies", {}, GooseDependenciesFunction, GooseDependenciesBind,
                                      GooseDependenciesInit));
    }
} // namespace goose
