#include <goose/parser/statement/relation_statement.h>
#include <goose/main/relation/query_relation.h>

namespace goose {

RelationStatement::RelationStatement(shared_ptr<Relation> relation_p)
    : SQLStatement(StatementType::RELATION_STATEMENT), relation(std::move(relation_p)) {
	query = relation->GetQuery();
}

unique_ptr<SQLStatement> RelationStatement::Copy() const {
	return unique_ptr<RelationStatement>(new RelationStatement(*this));
}

string RelationStatement::ToString() const {
	return relation->ToString();
}

} // namespace goose
