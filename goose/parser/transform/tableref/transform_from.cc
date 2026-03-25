#include <goose/parser/tableref/joinref.h>
#include <goose/parser/tableref/emptytableref.h>
#include <goose/parser/transformer.h>

namespace goose {
    unique_ptr<TableRef> Transformer::TransformFrom(optional_ptr<cantor::PGList> root) {
        if (!root) {
            return make_uniq<EmptyTableRef>();
        }

        if (root->length > 1) {
            // Implicit Cross Product
            auto result = make_uniq<JoinRef>(JoinRefType::CROSS);
            result->is_implicit = true;
            JoinRef *cur_root = result.get();
            idx_t list_size = 0;
            for (auto node = root->head; node != nullptr; node = node->next) {
                auto n = PGPointerCast<cantor::PGNode>(node->data.ptr_value);
                unique_ptr<TableRef> next = TransformTableRefNode(*n);
                if (!cur_root->left) {
                    cur_root->left = std::move(next);
                } else if (!cur_root->right) {
                    cur_root->right = std::move(next);
                } else {
                    auto old_res = std::move(result);
                    result = make_uniq<JoinRef>(JoinRefType::CROSS);
                    result->is_implicit = true;
                    result->left = std::move(old_res);
                    result->right = std::move(next);
                    cur_root = result.get();
                }
                list_size++;
                StackCheck(list_size);
            }
            return result;
        }

        auto n = PGPointerCast<cantor::PGNode>(root->head->data.ptr_value);
        return TransformTableRefNode(*n);
    }
} // namespace goose
