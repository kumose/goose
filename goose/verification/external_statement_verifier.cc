#include <goose/verification/external_statement_verifier.h>

namespace goose {
    ExternalStatementVerifier::ExternalStatementVerifier(
        unique_ptr<SQLStatement> statement_p, optional_ptr<case_insensitive_map_t<BoundParameterData> > parameters)
        : StatementVerifier(VerificationType::EXTERNAL, "External", std::move(statement_p), parameters) {
    }

    unique_ptr<StatementVerifier>
    ExternalStatementVerifier::Create(const SQLStatement &statement,
                                      optional_ptr<case_insensitive_map_t<BoundParameterData> > parameters) {
        return make_uniq < ExternalStatementVerifier > (statement.Copy(), parameters);
    }
} // namespace goose