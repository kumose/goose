#include <goose/parser/statement/detach_statement.h>
#include <goose/extension/autocomplete/transformer/peg_transformer.h>

namespace goose {
    unique_ptr<SQLStatement> PEGTransformerFactory::TransformDetachStatement(PEGTransformer &transformer,
                                                                             optional_ptr<ParseResult> parse_result) {
        auto result = make_uniq<DetachStatement>();
        auto info = make_uniq<DetachInfo>();

        auto &list_pr = parse_result->Cast<ListParseResult>();
        auto &if_exists = list_pr.Child<OptionalParseResult>(2);
        info->if_not_found = if_exists.optional_result
                                 ? OnEntryNotFound::RETURN_NULL
                                 : OnEntryNotFound::THROW_EXCEPTION;
        info->name = list_pr.Child<IdentifierParseResult>(3).identifier;
        result->info = std::move(info);
        return std::move(result);
    }
} // namespace goose
