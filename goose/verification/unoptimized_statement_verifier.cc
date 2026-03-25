#include <goose/verification/unoptimized_statement_verifier.h>

namespace goose {
    UnoptimizedStatementVerifier::UnoptimizedStatementVerifier(
        unique_ptr<SQLStatement> statement_p, optional_ptr<case_insensitive_map_t<BoundParameterData> > parameters)
        : StatementVerifier(VerificationType::UNOPTIMIZED, "Unoptimized", std::move(statement_p), parameters) {
    }

    unique_ptr<StatementVerifier>
    UnoptimizedStatementVerifier::Create(const SQLStatement &statement_p,
                                         optional_ptr<case_insensitive_map_t<BoundParameterData> > parameters) {
        return make_uniq < UnoptimizedStatementVerifier > (statement_p.Copy(), parameters);
    }
} // namespace goose