#include <goose/main/relation/cross_product_relation.h>
#include <goose/main/client_context.h>
#include <goose/parser/query_node/select_node.h>
#include <goose/parser/expression/star_expression.h>
#include <goose/parser/tableref/joinref.h>

namespace goose {
    CrossProductRelation::CrossProductRelation(shared_ptr<Relation> left_p, shared_ptr<Relation> right_p,
                                               JoinRefType ref_type)
        : Relation(left_p->context, RelationType::CROSS_PRODUCT_RELATION), left(std::move(left_p)),
          right(std::move(right_p)), ref_type(ref_type) {
        if (left->context->GetContext() != right->context->GetContext()) {
            throw InvalidInputException("Cannot combine LEFT and RIGHT relations of different connections!");
        }
        TryBindRelation(columns);
    }

    unique_ptr<QueryNode> CrossProductRelation::GetQueryNode() {
        auto result = make_uniq<SelectNode>();
        result->select_list.push_back(make_uniq<StarExpression>());
        result->from_table = GetTableRef();
        return result;
    }

    unique_ptr<TableRef> CrossProductRelation::GetTableRef() {
        auto cross_product_ref = make_uniq<JoinRef>(ref_type);
        cross_product_ref->left = left->GetTableRef();
        cross_product_ref->right = right->GetTableRef();
        return cross_product_ref;
    }

    const vector<ColumnDefinition> &CrossProductRelation::Columns() {
        return this->columns;
    }

    string CrossProductRelation::ToString(idx_t depth) {
        string str = RenderWhitespace(depth);
        str = "Cross Product";
        return str + "\n" + left->ToString(depth + 1) + right->ToString(depth + 1);
    }
} // namespace goose
