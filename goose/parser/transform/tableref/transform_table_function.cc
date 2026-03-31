#include <goose/common/exception.h>
#include <goose/parser/tableref/table_function_ref.h>
#include <goose/parser/transformer.h>

namespace goose {
    unique_ptr<TableRef> Transformer::TransformRangeFunction(cantor::PGRangeFunction &root) {
        if (root.is_rowsfrom) {
            throw NotImplementedException("ROWS FROM() not implemented");
        }
        if (root.functions->length != 1) {
            throw NotImplementedException("Need exactly one function");
        }
        auto function_sublist = PGPointerCast<cantor::PGList>(root.functions->head->data.ptr_value);
        D_ASSERT(function_sublist->length == 2);
        auto call_tree = PGPointerCast<cantor::PGNode>(function_sublist->head->data.ptr_value);
        auto coldef = function_sublist->head->next->data.ptr_value;

        if (coldef) {
            throw NotImplementedException("Explicit column definition not supported yet");
        }
        // transform the function call
        auto result = make_uniq<TableFunctionRef>();
        if (root.ordinality) {
            result->with_ordinality = OrdinalityType::WITH_ORDINALITY;
        }
        switch (call_tree->type) {
            case cantor::T_PGFuncCall: {
                auto func_call = PGPointerCast<cantor::PGFuncCall>(call_tree.get());
                result->function = TransformFuncCall(*func_call);
                SetQueryLocation(*result, func_call->location);
                break;
            }
            case cantor::T_PGSQLValueFunction:
                result->function =
                        TransformSQLValueFunction(*PGPointerCast<cantor::PGSQLValueFunction>(call_tree.get()));
                break;
            default:
                throw ParserException("Not a function call or value function");
        }
        result->alias = TransformAlias(root.alias, result->column_name_alias);
        if (root.sample) {
            result->sample = TransformSampleOptions(root.sample);
        }
        return std::move(result);
    }
} // namespace goose
