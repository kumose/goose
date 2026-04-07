#include <goose/function/table/system_functions.h>

#include <goose/common/exception.h>
#include <goose/main/client_context.h>
#include <goose/parser/parser.h>

namespace goose {
    struct GooseKeywordsData : public GlobalTableFunctionState {
        GooseKeywordsData() : offset(0) {
        }

        vector<ParserKeyword> entries;
        idx_t offset;
    };

    static unique_ptr<FunctionData> GooseKeywordsBind(ClientContext &context, TableFunctionBindInput &input,
                                                      vector<LogicalType> &return_types, vector<string> &names) {
        names.emplace_back("keyword_name");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("keyword_category");
        return_types.emplace_back(LogicalType::VARCHAR);

        return nullptr;
    }

    unique_ptr<GlobalTableFunctionState> GooseKeywordsInit(ClientContext &context, TableFunctionInitInput &input) {
        auto result = make_uniq<GooseKeywordsData>();
        result->entries = Parser::KeywordList();
        return std::move(result);
    }

    void GooseKeywordsFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
        auto &data = data_p.global_state->Cast<GooseKeywordsData>();
        if (data.offset >= data.entries.size()) {
            // finished returning values
            return;
        }
        // start returning values
        // either fill up the chunk or return all the remaining columns
        idx_t count = 0;
        while (data.offset < data.entries.size() && count < STANDARD_VECTOR_SIZE) {
            auto &entry = data.entries[data.offset++];

            // keyword_name, VARCHAR
            output.SetValue(0, count, Value(entry.name));
            // keyword_category, VARCHAR
            string category_name;
            switch (entry.category) {
                case KeywordCategory::KEYWORD_RESERVED:
                    category_name = "reserved";
                    break;
                case KeywordCategory::KEYWORD_UNRESERVED:
                    category_name = "unreserved";
                    break;
                case KeywordCategory::KEYWORD_TYPE_FUNC:
                    category_name = "type_function";
                    break;
                case KeywordCategory::KEYWORD_COL_NAME:
                    category_name = "column_name";
                    break;
                default:
                    throw InternalException("Unrecognized keyword category");
            }
            output.SetValue(1, count, Value(std::move(category_name)));

            count++;
        }
        output.SetCardinality(count);
    }

    void GooseKeywordsFun::RegisterFunction(BuiltinFunctions &set) {
        set.AddFunction(
            TableFunction("goose_keywords", {}, GooseKeywordsFunction, GooseKeywordsBind, GooseKeywordsInit));
    }
} // namespace goose
