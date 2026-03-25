#include <goose/parser/tableref/subqueryref.h>
#include <goose/parser/transformer.h>

namespace goose {
    unique_ptr<TableRef> Transformer::TransformRangeSubselect(cantor::PGRangeSubselect &root) {
        Transformer subquery_transformer(*this);
        auto subquery = subquery_transformer.TransformSelectStmt(*root.subquery);
        if (!subquery) {
            return nullptr;
        }
        auto result = make_uniq<SubqueryRef>(std::move(subquery));
        result->alias = TransformAlias(root.alias, result->column_name_alias);
        if (root.sample) {
            result->sample = TransformSampleOptions(root.sample);
        }
        return result;
    }
} // namespace goose
