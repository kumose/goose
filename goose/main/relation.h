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

#include <goose/common/common.h>
#include <goose/common/enums/join_type.h>
#include <goose/common/enums/relation_type.h>
#include <goose/common/winapi.h>
#include <goose/common/enums/joinref_type.h>
#include <goose/main/query_result.h>
#include <goose/parser/column_definition.h>
#include <goose/common/named_parameter_map.h>
#include <goose/main/client_context.h>
#include <goose/main/client_context_wrapper.h>
#include <goose/main/external_dependencies.h>
#include <goose/parser/statement/explain_statement.h>
#include <goose/parser/parsed_expression.h>
#include <goose/parser/result_modifier.h>
#include <goose/common/unique_ptr.h>
#include <goose/common/vector.h>
#include <goose/common/helper.h>

namespace goose {
struct BoundStatement;

class Binder;
class LogicalOperator;
class QueryNode;
class TableRef;

static string CreateRelationAlias(RelationType type, const string &alias) {
	if (!alias.empty()) {
		return alias;
	}
	return StringUtil::Format("%s_%s", EnumUtil::ToString(type), StringUtil::GenerateRandomName());
}

class RelationContextWrapper : public ClientContextWrapper {
public:
	~RelationContextWrapper() override = default;
	explicit RelationContextWrapper(const shared_ptr<ClientContext> &context) : ClientContextWrapper(context) {};

	explicit RelationContextWrapper(const ClientContextWrapper &context) : ClientContextWrapper(context) {};

	void TryBindRelation(Relation &relation, vector<ColumnDefinition> &columns) override {
		GetContext()->TryBindRelation(relation, columns);
	}

private:
	weak_ptr<ClientContext> client_context;
};

class Relation : public enable_shared_from_this<Relation> {
public:
	Relation(const shared_ptr<ClientContext> &context_p, const RelationType type) : type(type) {
		context = make_shared_ptr<ClientContextWrapper>(context_p);
	}
	Relation(const shared_ptr<ClientContextWrapper> &context, RelationType type, const string &alias_p = "")
	    : context(context), type(type), alias(CreateRelationAlias(type, alias_p)) {
	}

	Relation(const shared_ptr<RelationContextWrapper> &context, RelationType type, const string &alias_p = "")
	    : context(context), type(type), alias(CreateRelationAlias(type, alias_p)) {
	}

	virtual ~Relation() = default;

	shared_ptr<ClientContextWrapper> context;
	RelationType type;
	const string alias;
	vector<shared_ptr<ExternalDependency>> external_dependencies;

public:
	GOOSE_API virtual const vector<ColumnDefinition> &Columns() = 0;
	GOOSE_API virtual unique_ptr<QueryNode> GetQueryNode() = 0;
	GOOSE_API virtual string GetQuery();
	GOOSE_API virtual BoundStatement Bind(Binder &binder);
	GOOSE_API virtual string GetAlias();

	GOOSE_API unique_ptr<QueryResult> ExecuteOrThrow();
	GOOSE_API unique_ptr<QueryResult> Execute();
	GOOSE_API string ToString();
	GOOSE_API virtual string ToString(idx_t depth) = 0;

	GOOSE_API void Print();
	GOOSE_API void Head(idx_t limit = 10);

	GOOSE_API shared_ptr<Relation> CreateView(const string &name, bool replace = true, bool temporary = false);
	GOOSE_API shared_ptr<Relation> CreateView(const string &schema_name, const string &name, bool replace = true,
	                                           bool temporary = false);
	GOOSE_API unique_ptr<QueryResult> Query(const string &sql) const;
	GOOSE_API unique_ptr<QueryResult> Query(const string &name, const string &sql);

	//! Explain the query plan of this relation
	GOOSE_API unique_ptr<QueryResult> Explain(ExplainType type = ExplainType::EXPLAIN_STANDARD,
	                                           ExplainFormat explain_format = ExplainFormat::DEFAULT);

	GOOSE_API virtual unique_ptr<TableRef> GetTableRef();
	virtual bool IsReadOnly() {
		return true;
	}
	GOOSE_API void TryBindRelation(vector<ColumnDefinition> &columns);

public:
	// PROJECT
	GOOSE_API shared_ptr<Relation> Project(const string &select_list);
	GOOSE_API shared_ptr<Relation> Project(const string &expression, const string &alias);
	GOOSE_API shared_ptr<Relation> Project(const string &select_list, const vector<string> &aliases);
	GOOSE_API shared_ptr<Relation> Project(const vector<string> &expressions);
	GOOSE_API shared_ptr<Relation> Project(const vector<string> &expressions, const vector<string> &aliases);
	GOOSE_API shared_ptr<Relation> Project(vector<unique_ptr<ParsedExpression>> expressions,
	                                        const vector<string> &aliases);

	// FILTER
	GOOSE_API shared_ptr<Relation> Filter(const string &expression);
	GOOSE_API shared_ptr<Relation> Filter(unique_ptr<ParsedExpression> expression);
	GOOSE_API shared_ptr<Relation> Filter(const vector<string> &expressions);

	// LIMIT
	GOOSE_API shared_ptr<Relation> Limit(int64_t n, int64_t offset = 0);

	// ORDER
	GOOSE_API shared_ptr<Relation> Order(const string &expression);
	GOOSE_API shared_ptr<Relation> Order(const vector<string> &expressions);
	GOOSE_API shared_ptr<Relation> Order(vector<OrderByNode> expressions);

	// JOIN operation
	GOOSE_API shared_ptr<Relation> Join(const shared_ptr<Relation> &other, const string &condition,
	                                     JoinType type = JoinType::INNER, JoinRefType ref_type = JoinRefType::REGULAR);
	shared_ptr<Relation> Join(const shared_ptr<Relation> &other, vector<unique_ptr<ParsedExpression>> condition,
	                          JoinType type = JoinType::INNER, JoinRefType ref_type = JoinRefType::REGULAR);

	// CROSS PRODUCT operation
	GOOSE_API shared_ptr<Relation> CrossProduct(const shared_ptr<Relation> &other,
	                                             JoinRefType join_ref_type = JoinRefType::CROSS);

	// SET operations
	GOOSE_API shared_ptr<Relation> Union(const shared_ptr<Relation> &other);
	GOOSE_API shared_ptr<Relation> Except(const shared_ptr<Relation> &other);
	GOOSE_API shared_ptr<Relation> Intersect(const shared_ptr<Relation> &other);

	// DISTINCT operation
	GOOSE_API shared_ptr<Relation> Distinct();

	// AGGREGATES
	GOOSE_API shared_ptr<Relation> Aggregate(const string &aggregate_list);
	GOOSE_API shared_ptr<Relation> Aggregate(const vector<string> &aggregates);
	GOOSE_API shared_ptr<Relation> Aggregate(vector<unique_ptr<ParsedExpression>> expressions);
	GOOSE_API shared_ptr<Relation> Aggregate(const string &aggregate_list, const string &group_list);
	GOOSE_API shared_ptr<Relation> Aggregate(const vector<string> &aggregates, const vector<string> &groups);
	GOOSE_API shared_ptr<Relation> Aggregate(vector<unique_ptr<ParsedExpression>> expressions,
	                                          const string &group_list);

	// ALIAS
	GOOSE_API shared_ptr<Relation> Alias(const string &alias);

	//! Insert the data from this relation into a table
	GOOSE_API shared_ptr<Relation> InsertRel(const string &schema_name, const string &table_name);
	GOOSE_API shared_ptr<Relation> InsertRel(const string &catalog_name, const string &schema_name,
	                                          const string &table_name);
	GOOSE_API void Insert(const string &table_name);
	GOOSE_API void Insert(const string &schema_name, const string &table_name);
	GOOSE_API void Insert(const string &catalog_name, const string &schema_name, const string &table_name);
	//! Insert a row (i.e.,list of values) into a table
	GOOSE_API virtual void Insert(const vector<vector<Value>> &values);
	GOOSE_API virtual void Insert(vector<vector<unique_ptr<ParsedExpression>>> &&expressions);
	//! Create a table and insert the data from this relation into that table
	GOOSE_API shared_ptr<Relation> CreateRel(const string &schema_name, const string &table_name,
	                                          bool temporary = false,
	                                          OnCreateConflict on_conflict = OnCreateConflict::ERROR_ON_CONFLICT);
	GOOSE_API shared_ptr<Relation> CreateRel(const string &catalog_name, const string &schema_name,
	                                          const string &table_name, bool temporary = false,
	                                          OnCreateConflict on_conflict = OnCreateConflict::ERROR_ON_CONFLICT);
	GOOSE_API void Create(const string &table_name, bool temporary = false,
	                       OnCreateConflict on_conflict = OnCreateConflict::ERROR_ON_CONFLICT);
	GOOSE_API void Create(const string &schema_name, const string &table_name, bool temporary = false,
	                       OnCreateConflict on_conflict = OnCreateConflict::ERROR_ON_CONFLICT);
	GOOSE_API void Create(const string &catalog_name, const string &schema_name, const string &table_name,
	                       bool temporary = false, OnCreateConflict on_conflict = OnCreateConflict::ERROR_ON_CONFLICT);

	//! Write a relation to a CSV file
	GOOSE_API shared_ptr<Relation>
	WriteCSVRel(const string &csv_file,
	            case_insensitive_map_t<vector<Value>> options = case_insensitive_map_t<vector<Value>>());
	GOOSE_API void WriteCSV(const string &csv_file,
	                         case_insensitive_map_t<vector<Value>> options = case_insensitive_map_t<vector<Value>>());
	//! Write a relation to a Parquet file
	GOOSE_API shared_ptr<Relation>
	WriteParquetRel(const string &parquet_file,
	                case_insensitive_map_t<vector<Value>> options = case_insensitive_map_t<vector<Value>>());
	GOOSE_API void
	WriteParquet(const string &parquet_file,
	             case_insensitive_map_t<vector<Value>> options = case_insensitive_map_t<vector<Value>>());

	//! Update a table, can only be used on a TableRelation
	GOOSE_API virtual void Update(const string &update, const string &condition = string());
	GOOSE_API virtual void Update(vector<string> column_names, vector<unique_ptr<ParsedExpression>> &&update,
	                               unique_ptr<ParsedExpression> condition = nullptr);
	//! Delete from a table, can only be used on a TableRelation
	GOOSE_API virtual void Delete(const string &condition = string());
	//! Create a relation from calling a table in/out function on the input relation
	//! Create a relation from calling a table in/out function on the input relation
	GOOSE_API shared_ptr<Relation> TableFunction(const std::string &fname, const vector<Value> &values);
	GOOSE_API shared_ptr<Relation> TableFunction(const std::string &fname, const vector<Value> &values,
	                                              const named_parameter_map_t &named_parameters);

public:
	//! Whether or not the relation inherits column bindings from its child or not, only relevant for binding
	virtual bool InheritsColumnBindings() {
		return false;
	}
	virtual Relation *ChildRelation() {
		return nullptr;
	}
	void AddExternalDependency(shared_ptr<ExternalDependency> dependency);
	GOOSE_API vector<shared_ptr<ExternalDependency>> GetAllDependencies();

protected:
	GOOSE_API static string RenderWhitespace(idx_t depth);

public:
	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}
};

} // namespace goose
