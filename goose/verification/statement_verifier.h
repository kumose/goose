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

#include <goose/main/client_context.h>
#include <goose/main/materialized_query_result.h>
#include <goose/parser/statement/select_statement.h>

namespace goose {
    enum class VerificationType : uint8_t {
        ORIGINAL,
        COPIED,
        DESERIALIZED,
        PARSED,
        UNOPTIMIZED,
        NO_OPERATOR_CACHING,
        PREPARED,
        EXTERNAL,
        EXPLAIN,
        FETCH_ROW_AS_SCAN,

        INVALID
    };

    class StatementVerifier {
    public:
        StatementVerifier(VerificationType type, string name, unique_ptr<SQLStatement> statement_p,
                          optional_ptr<case_insensitive_map_t<BoundParameterData> > values);

        explicit StatementVerifier(unique_ptr<SQLStatement> statement_p,
                                   optional_ptr<case_insensitive_map_t<BoundParameterData> > values);

        static unique_ptr<StatementVerifier> Create(VerificationType type, const SQLStatement &statement_p,
                                                    optional_ptr<case_insensitive_map_t<BoundParameterData> > values);

        virtual ~StatementVerifier() noexcept;

    public:
        //! Check whether expressions in this verifier and the other verifier match
        void CheckExpressions(const StatementVerifier &other) const;

        //! Check whether expressions within this verifier match
        void CheckExpressions() const;

        //! Run the select statement and store the result
        virtual bool
        Run(ClientContext &context, const string &query,
            const std::function<unique_ptr<QueryResult>(const string &, unique_ptr<SQLStatement>,
                                                        optional_ptr<case_insensitive_map_t<BoundParameterData> >)> &
            run);

        //! Compare this verifier's results with another verifier
        string CompareResults(const StatementVerifier &other);

    public:
        virtual bool RequireEquality() const {
            return true;
        }

        virtual bool DisableOptimizer() const {
            return false;
        }

        virtual bool DisableOperatorCaching() const {
            return false;
        }

        virtual bool ForceExternal() const {
            return false;
        }

        virtual bool ForceFetchRow() const {
            return false;
        }

    public:
        const VerificationType type;
        const string name;
        unique_ptr<SQLStatement> statement;
        optional_ptr<SelectStatement> select_statement;
        optional_ptr<case_insensitive_map_t<BoundParameterData> > parameters;
        const vector<unique_ptr<ParsedExpression> > &select_list;
        unique_ptr<MaterializedQueryResult> materialized_result;

    private:
        const vector<unique_ptr<ParsedExpression> > empty_select_list = {};

        const vector<unique_ptr<ParsedExpression> > &GetSelectList(QueryNode &node);
    };
} // namespace goose