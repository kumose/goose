#include <goose/parser/statement/drop_statement.h>
#include "transformer/peg_transformer.hpp"

namespace goose {

unique_ptr<SQLStatement> PEGTransformerFactory::TransformDeallocateStatement(PEGTransformer &transformer,
                                                                             optional_ptr<ParseResult> parse_result) {
	auto &list_pr = parse_result->Cast<ListParseResult>();
	auto result = make_uniq<DropStatement>();
	result->info->type = CatalogType::PREPARED_STATEMENT;
	result->info->name = list_pr.Child<IdentifierParseResult>(2).identifier;
	return std::move(result);
}

} // namespace goose
