// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <goose/verification/statement_verifier.h>

namespace goose {
    class PreparedStatementVerifier : public StatementVerifier {
    public:
        explicit PreparedStatementVerifier(unique_ptr<SQLStatement> statement_p,
                                           optional_ptr<case_insensitive_map_t<BoundParameterData> > parameters);

        static unique_ptr<StatementVerifier> Create(const SQLStatement &statement_p,
                                                    optional_ptr<case_insensitive_map_t<BoundParameterData> >
                                                    parameters);

        bool Run(ClientContext &context, const string &query,
                 const std::function<unique_ptr<QueryResult>(const string &, unique_ptr<SQLStatement>,
                                                             optional_ptr<case_insensitive_map_t<BoundParameterData> >)>
                 &run) override;

    private:
        case_insensitive_map_t<unique_ptr<ParsedExpression> > values;
        unique_ptr<SQLStatement> prepare_statement;
        unique_ptr<SQLStatement> execute_statement;
        unique_ptr<SQLStatement> dealloc_statement;

    private:
        void Extract();

        void ConvertConstants(unique_ptr<ParsedExpression> &child);
    };
} // namespace goose