// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <goose/common/case_insensitive_map.h>
#include <goose/common/constants.h>
#include <goose/common/stack_checker.h>
#include <goose/common/types.h>
#include <goose/common/types-import.h>
#include <goose/parser/group_by_node.h>
#include <goose/parser/parsed_data/create_info.h>
#include <goose/parser/parsed_data/create_secret_info.h>
#include <goose/parser/qualified_name.h>
#include <goose/parser/query_node.h>
#include <goose/parser/tokens.h>
#include <cantor/nodes/parsenodes.h>
#include <cantor/nodes/primnodes.h>
#include <cantor/definitions.h>
#include <goose/parser/expression/parameter_expression.h>
#include <goose/common/enums/on_entry_not_found.h>

namespace goose {

class ColumnDefinition;
struct OrderByNode;
struct CopyInfo;
struct CommonTableExpressionInfo;
struct GroupingExpressionMap;
class OnConflictInfo;
class UpdateSetInfo;
class MacroFunction;
struct ParserOptions;
struct PivotColumn;
struct PivotColumnEntry;

//! The transformer class is responsible for transforming the internal Postgres
//! parser representation into the Goose representation
class Transformer {
	friend class StackChecker<Transformer>;

	struct CreatePivotEntry {
		string enum_name;
		unique_ptr<SelectNode> base;
		unique_ptr<ParsedExpression> column;
		unique_ptr<QueryNode> subquery;
		bool has_parameters;
	};

public:
	explicit Transformer(ParserOptions &options);
	Transformer(Transformer &parent);
	~Transformer();

	//! Transforms a Postgres parse tree into a set of SQL Statements
	bool TransformParseTree(cantor::PGList *tree, vector<unique_ptr<SQLStatement>> &statements);
	string NodetypeToString(cantor::PGNodeTag type);

	idx_t ParamCount() const;

private:
	optional_ptr<Transformer> parent;
	//! Parser options
	ParserOptions &options;
	//! The current prepared statement parameter index
	idx_t prepared_statement_parameter_index = 0;
	//! Map from named parameter to parameter index;
	case_insensitive_map_t<idx_t> named_param_map;
	//! Last parameter type
	PreparedParamType last_param_type = PreparedParamType::INVALID;
	//! Holds window expressions defined by name. We need those when transforming the expressions referring to them.
	case_insensitive_map_t<cantor::PGWindowDef *> window_clauses;
	//! The set of pivot entries to create
	vector<unique_ptr<CreatePivotEntry>> pivot_entries;
	//! Sets of stored CTEs, if any
	vector<reference<CommonTableExpressionMap>> stored_cte_map;
	//! Whether or not we are currently binding a window definition
	bool in_window_definition = false;

	void Clear();
	bool InWindowDefinition();

	Transformer &RootTransformer();
	const Transformer &RootTransformer() const;
	void SetParamCount(idx_t new_count);
	void ClearParameters();
	void SetParam(const string &name, idx_t index, PreparedParamType type);
	bool GetParam(const string &name, idx_t &index, PreparedParamType type);

	void AddPivotEntry(string enum_name, unique_ptr<SelectNode> source, unique_ptr<ParsedExpression> column,
	                   unique_ptr<QueryNode> subquery, bool has_parameters);
	unique_ptr<SQLStatement> GenerateCreateEnumStmt(unique_ptr<CreatePivotEntry> entry);
	bool HasPivotEntries();
	idx_t PivotEntryCount();
	vector<unique_ptr<CreatePivotEntry>> &GetPivotEntries();
	void PivotEntryCheck(const string &type);
	void ExtractCTEsRecursive(CommonTableExpressionMap &cte_map);

private:
	//! Transforms a Postgres statement into a single SQL statement
	unique_ptr<SQLStatement> TransformStatement(cantor::PGNode &stmt);
	//! Transforms a Postgres statement into a single SQL statement
	unique_ptr<SQLStatement> TransformStatementInternal(cantor::PGNode &stmt);
	//===--------------------------------------------------------------------===//
	// Statement transformation
	//===--------------------------------------------------------------------===//
	//! Transform a Postgres cantor::T_PGSelectStmt node into a SelectStatement
	unique_ptr<SelectStatement> TransformSelectStmt(cantor::PGSelectStmt &select, bool is_select = true);
	unique_ptr<SelectStatement> TransformSelectStmt(cantor::PGNode &node, bool is_select = true);
	//! Transform a Postgres T_AlterStmt node into a AlterStatement
	unique_ptr<SQLStatement> TransformAlter(cantor::PGAlterTableStmt &stmt);
	//! Transform a Postgres T_AlterDatabaseStmt node into a AlterStatement
	unique_ptr<AlterStatement> TransformAlterDatabase(cantor::PGAlterDatabaseStmt &stmt);
	//! Transform a Postgres cantor::T_PGRenameStmt node into a RenameStatement
	unique_ptr<AlterStatement> TransformRename(cantor::PGRenameStmt &stmt);
	//! Transform a Postgres cantor::T_PGCreateStmt node into a CreateStatement
	unique_ptr<CreateStatement> TransformCreateTable(cantor::PGCreateStmt &node);
	//! Transform a Postgres cantor::T_PGCreateStmt node into a CreateStatement
	unique_ptr<CreateStatement> TransformCreateTableAs(cantor::PGCreateTableAsStmt &stmt);
	//! Transform a Postgres node into a CreateStatement
	unique_ptr<CreateStatement> TransformCreateSchema(cantor::PGCreateSchemaStmt &stmt);
	//! Transform a Postgres cantor::T_PGCreateSeqStmt node into a CreateStatement
	unique_ptr<CreateStatement> TransformCreateSequence(cantor::PGCreateSeqStmt &node);
	//! Transform a Postgres cantor::T_PGViewStmt node into a CreateStatement
	unique_ptr<CreateStatement> TransformCreateView(cantor::PGViewStmt &node);
	//! Transform a Postgres cantor::T_PGIndexStmt node into CreateStatement
	unique_ptr<CreateStatement> TransformCreateIndex(cantor::PGIndexStmt &stmt);
	//! Transform a Postgres cantor::T_PGCreateFunctionStmt node into CreateStatement
	unique_ptr<CreateStatement> TransformCreateFunction(cantor::PGCreateFunctionStmt &stmt);
	//! Transform a Postgres cantor::T_PGCreateTypeStmt node into CreateStatement
	unique_ptr<CreateStatement> TransformCreateType(cantor::PGCreateTypeStmt &stmt);
	//! Transform a Postgres cantor::T_PGCreateTypeStmt node into CreateStatement
	unique_ptr<AlterStatement> TransformCommentOn(cantor::PGCommentOnStmt &stmt);
	//! Transform a Postgres cantor::T_PGAlterSeqStmt node into CreateStatement
	unique_ptr<AlterStatement> TransformAlterSequence(cantor::PGAlterSeqStmt &stmt);
	//! Transform a Postgres cantor::T_PGDropStmt node into a Drop[Table,Schema]Statement
	unique_ptr<SQLStatement> TransformDrop(cantor::PGDropStmt &stmt);
	//! Transform a Postgres cantor::T_PGInsertStmt node into a InsertStatement
	unique_ptr<InsertStatement> TransformInsert(cantor::PGInsertStmt &stmt);
	InsertColumnOrder TransformColumnOrder(cantor::PGInsertColumnOrder insert_column_order);

	vector<string> TransformInsertColumns(cantor::PGList &cols);

	//! Transform a Postgres cantor::T_PGOnConflictClause node into a OnConflictInfo
	unique_ptr<OnConflictInfo> TransformOnConflictClause(cantor::PGOnConflictClause *node,
	                                                     const string &relname);
	//! Transform a ON CONFLICT shorthand into a OnConflictInfo
	unique_ptr<OnConflictInfo> DummyOnConflictClause(cantor::PGOnConflictActionAlias type,
	                                                 const string &relname);
	//! Transform a Postgres cantor::T_PGCopyStmt node into a CopyStatement
	unique_ptr<CopyStatement> TransformCopy(cantor::PGCopyStmt &stmt);
	void TransformCopyOptions(CopyInfo &info, optional_ptr<cantor::PGList> options);
	void TransformCreateSecretOptions(CreateSecretInfo &info, optional_ptr<cantor::PGList> options);
	//! Transform a Postgres cantor::T_PGTransactionStmt node into a TransactionStatement
	unique_ptr<TransactionStatement> TransformTransaction(cantor::PGTransactionStmt &stmt);
	//! Transform a Postgres T_DeleteStatement node into a DeleteStatement
	unique_ptr<DeleteStatement> TransformDelete(cantor::PGDeleteStmt &stmt);
	//! Transform a Postgres cantor::T_PGUpdateStmt node into a UpdateStatement
	unique_ptr<UpdateStatement> TransformUpdate(cantor::PGUpdateStmt &stmt);
	//! Transform a Postgres cantor::T_PGUpdateExtensionsStmt node into a UpdateExtensionsStatement
	unique_ptr<UpdateExtensionsStatement> TransformUpdateExtensions(cantor::PGUpdateExtensionsStmt &stmt);
	//! Transform a Postgres cantor::T_PGPragmaStmt node into a PragmaStatement
	unique_ptr<SQLStatement> TransformPragma(cantor::PGPragmaStmt &stmt);
	//! Transform a Postgres cantor::T_PGExportStmt node into a ExportStatement
	unique_ptr<ExportStatement> TransformExport(cantor::PGExportStmt &stmt);
	//! Transform a Postgres cantor::T_PGImportStmt node into a PragmaStatement
	unique_ptr<PragmaStatement> TransformImport(cantor::PGImportStmt &stmt);
	unique_ptr<ExplainStatement> TransformExplain(cantor::PGExplainStmt &stmt);
	unique_ptr<SQLStatement> TransformVacuum(cantor::PGVacuumStmt &stmt);
	unique_ptr<QueryNode> TransformShow(cantor::PGVariableShowStmt &stmt);
	unique_ptr<SelectStatement> TransformShowStmt(cantor::PGVariableShowStmt &stmt);
	unique_ptr<QueryNode> TransformShowSelect(cantor::PGVariableShowSelectStmt &stmt);
	unique_ptr<SelectStatement> TransformShowSelectStmt(cantor::PGVariableShowSelectStmt &stmt);
	unique_ptr<AttachStatement> TransformAttach(cantor::PGAttachStmt &stmt);
	unique_ptr<DetachStatement> TransformDetach(cantor::PGDetachStmt &stmt);
	unique_ptr<SetStatement> TransformUse(cantor::PGUseStmt &stmt);
	unique_ptr<SQLStatement> TransformCopyDatabase(cantor::PGCopyDatabaseStmt &stmt);
	unique_ptr<CreateStatement> TransformSecret(cantor::PGCreateSecretStmt &stmt);
	unique_ptr<DropStatement> TransformDropSecret(cantor::PGDropSecretStmt &stmt);

	unique_ptr<PrepareStatement> TransformPrepare(cantor::PGPrepareStmt &stmt);
	unique_ptr<ExecuteStatement> TransformExecute(cantor::PGExecuteStmt &stmt);
	unique_ptr<CallStatement> TransformCall(cantor::PGCallStmt &stmt);
	unique_ptr<DropStatement> TransformDeallocate(cantor::PGDeallocateStmt &stmt);
	unique_ptr<QueryNode> TransformPivotStatement(cantor::PGSelectStmt &select);
	unique_ptr<SQLStatement> CreatePivotStatement(unique_ptr<SQLStatement> statement);
	PivotColumn TransformPivotColumn(cantor::PGPivot &pivot, bool is_pivot);
	vector<PivotColumn> TransformPivotList(cantor::PGList &list, bool is_pivot);
	static bool TransformPivotInList(unique_ptr<ParsedExpression> &expr, PivotColumnEntry &entry,
	                                 bool root_entry = true);

	unique_ptr<SQLStatement> TransformMergeInto(cantor::PGMergeIntoStmt &stmt);
	unique_ptr<MergeIntoAction> TransformMergeIntoAction(cantor::PGMatchAction &action);

	//===--------------------------------------------------------------------===//
	// SetStatement Transform
	//===--------------------------------------------------------------------===//
	unique_ptr<SetStatement> TransformSet(cantor::PGVariableSetStmt &set);
	unique_ptr<SetStatement> TransformSetVariable(cantor::PGVariableSetStmt &stmt);
	unique_ptr<SetStatement> TransformResetVariable(cantor::PGVariableSetStmt &stmt);

	unique_ptr<SQLStatement> TransformCheckpoint(cantor::PGCheckPointStmt &stmt);
	unique_ptr<LoadStatement> TransformLoad(cantor::PGLoadStmt &stmt);

	//===--------------------------------------------------------------------===//
	// Query Node Transform
	//===--------------------------------------------------------------------===//
	//! Transform a Postgres cantor::T_PGSelectStmt node into a QueryNode
	unique_ptr<QueryNode> TransformSelectNode(cantor::PGNode &select, bool is_select = true);
	unique_ptr<QueryNode> TransformSelectNodeInternal(cantor::PGSelectStmt &select, bool is_select = true);
	unique_ptr<QueryNode> TransformSelectInternal(cantor::PGSelectStmt &select);
	void TransformModifiers(cantor::PGSelectStmt &stmt, QueryNode &node);
	bool SetOperationsMatch(cantor::PGSelectStmt &root, cantor::PGNode &node);
	void TransformSetOperationChildren(cantor::PGSelectStmt &stmt, SetOperationNode &result);

	//===--------------------------------------------------------------------===//
	// Expression Transform
	//===--------------------------------------------------------------------===//
	//! Transform a Postgres boolean expression into an Expression
	unique_ptr<ParsedExpression> TransformBoolExpr(cantor::PGBoolExpr &root);
	//! Transform a Postgres case expression into an Expression
	unique_ptr<ParsedExpression> TransformCase(cantor::PGCaseExpr &root);
	//! Transform a Postgres type cast into an Expression
	unique_ptr<ParsedExpression> TransformTypeCast(cantor::PGTypeCast &root);
	//! Transform a Postgres coalesce into an Expression
	unique_ptr<ParsedExpression> TransformCoalesce(cantor::PGAExpr &root);
	//! Transform a Postgres column reference into an Expression
	unique_ptr<ParsedExpression> TransformColumnRef(cantor::PGColumnRef &root);
	//! Transform a Postgres constant value into an Expression
	unique_ptr<ConstantExpression> TransformValue(cantor::PGValue val);
	//! Transform a Postgres operator into an Expression
	unique_ptr<ParsedExpression> TransformAExpr(cantor::PGAExpr &root);
	unique_ptr<ParsedExpression> TransformAExprInternal(cantor::PGAExpr &root);
	//! Transform a Postgres abstract expression into an Expression
	unique_ptr<ParsedExpression> TransformExpression(optional_ptr<cantor::PGNode> node);
	unique_ptr<ParsedExpression> TransformExpression(cantor::PGNode &node);
	//! Transform a Postgres function call into an Expression
	unique_ptr<ParsedExpression> TransformFuncCall(cantor::PGFuncCall &root);
	//! Transform a Postgres boolean expression into an Expression
	unique_ptr<ParsedExpression> TransformInterval(cantor::PGIntervalConstant &root);
	//! Transform a LAMBDA node (e.g., lambda x, y: x + y) into a lambda expression.
	unique_ptr<ParsedExpression> TransformLambda(cantor::PGLambdaFunction &node);
	//! Transform a single arrow operator (e.g., (x, y) -> x + y) into a lambda expression.
	unique_ptr<ParsedExpression> TransformSingleArrow(cantor::PGSingleArrowFunction &node);
	//! Transform a Postgres array access node (e.g. x[1] or x[1:3])
	unique_ptr<ParsedExpression> TransformArrayAccess(cantor::PGAIndirection &node);
	//! Transform a positional reference (e.g. #1)
	unique_ptr<ParsedExpression> TransformPositionalReference(cantor::PGPositionalReference &node);
	unique_ptr<ParsedExpression> TransformStarExpression(cantor::PGAStar &node);
	unique_ptr<ParsedExpression> TransformBooleanTest(cantor::PGBooleanTest &node);

	//! Transform a Postgres constant value into an Expression
	unique_ptr<ParsedExpression> TransformConstant(cantor::PGAConst &c);
	unique_ptr<ParsedExpression> TransformGroupingFunction(cantor::PGGroupingFunc &n);
	unique_ptr<ParsedExpression> TransformResTarget(cantor::PGResTarget &root);
	unique_ptr<ParsedExpression> TransformNullTest(cantor::PGNullTest &root);
	unique_ptr<ParsedExpression> TransformParamRef(cantor::PGParamRef &node);
	unique_ptr<ParsedExpression> TransformNamedArg(cantor::PGNamedArgExpr &root);

	//! Transform multi assignment reference into an Expression
	unique_ptr<ParsedExpression> TransformMultiAssignRef(cantor::PGMultiAssignRef &root);

	unique_ptr<ParsedExpression> TransformSQLValueFunction(cantor::PGSQLValueFunction &node);

	unique_ptr<ParsedExpression> TransformSubquery(cantor::PGSubLink &root);
	//===--------------------------------------------------------------------===//
	// Constraints transform
	//===--------------------------------------------------------------------===//
	unique_ptr<Constraint> TransformConstraint(cantor::PGConstraint &constraint);
	unique_ptr<Constraint> TransformConstraint(cantor::PGConstraint &constraint, ColumnDefinition &column,
	                                           idx_t index);

	//===--------------------------------------------------------------------===//
	// Update transform
	//===--------------------------------------------------------------------===//
	unique_ptr<UpdateSetInfo> TransformUpdateSetInfo(cantor::PGList *target_list,
	                                                 cantor::PGNode *where_clause);

	//===--------------------------------------------------------------------===//
	// Index transform
	//===--------------------------------------------------------------------===//
	vector<unique_ptr<ParsedExpression>> TransformIndexParameters(cantor::PGList &list,
	                                                              const string &relation_name);

	//===--------------------------------------------------------------------===//
	// Collation transform
	//===--------------------------------------------------------------------===//
	unique_ptr<ParsedExpression> TransformCollateExpr(cantor::PGCollateClause &collate);

	string TransformCollation(optional_ptr<cantor::PGCollateClause> collate);

	ColumnDefinition TransformColumnDefinition(cantor::PGColumnDef &cdef);
	//===--------------------------------------------------------------------===//
	// Helpers
	//===--------------------------------------------------------------------===//
	OnCreateConflict TransformOnConflict(cantor::PGOnCreateConflict conflict);
	string TransformAlias(cantor::PGAlias *root, vector<string> &column_name_alias);
	vector<string> TransformStringList(cantor::PGList *list);
	void TransformCTE(cantor::PGWithClause &de_with_clause, CommonTableExpressionMap &cte_map);
	unique_ptr<SelectStatement> TransformRecursiveCTE(cantor::PGCommonTableExpr &node,
	                                                  CommonTableExpressionInfo &info);

	unique_ptr<ParsedExpression> TransformUnaryOperator(const string &op, unique_ptr<ParsedExpression> child);
	unique_ptr<ParsedExpression> TransformBinaryOperator(string op, unique_ptr<ParsedExpression> left,
	                                                     unique_ptr<ParsedExpression> right);
	static bool ConstructConstantFromExpression(const ParsedExpression &expr, Value &value);
	//===--------------------------------------------------------------------===//
	// TableRef transform
	//===--------------------------------------------------------------------===//
	//! Transform a Postgres node into a TableRef
	unique_ptr<TableRef> TransformTableRefNode(cantor::PGNode &n);
	//! Transform a Postgres FROM clause into a TableRef
	unique_ptr<TableRef> TransformFrom(optional_ptr<cantor::PGList> root);
	//! Transform a Postgres table reference into a TableRef
	unique_ptr<TableRef> TransformRangeVar(cantor::PGRangeVar &root);
	//! Transform a Postgres table-producing function into a TableRef
	unique_ptr<TableRef> TransformRangeFunction(cantor::PGRangeFunction &root);
	//! Transform a Postgres join node into a TableRef
	unique_ptr<TableRef> TransformJoin(cantor::PGJoinExpr &root);
	//! Transform a Postgres pivot node into a TableRef
	unique_ptr<TableRef> TransformPivot(cantor::PGPivotExpr &root);
	//! Transform a table producing subquery into a TableRef
	unique_ptr<TableRef> TransformRangeSubselect(cantor::PGRangeSubselect &root);
	//! Transform a VALUES list into a set of expressions
	unique_ptr<TableRef> TransformValuesList(cantor::PGList *list);

	//! Transform using clause
	vector<string> TransformUsingClause(cantor::PGList &usingClause);

	//! Transform a range var into a (schema) qualified name
	QualifiedName TransformQualifiedName(cantor::PGRangeVar &root);

	//! Transform a Postgres TypeName string into a LogicalType (non-LIST types)
	LogicalType TransformTypeNameInternal(cantor::PGTypeName &name);
	//! Transform a Postgres TypeName string into a LogicalType
	LogicalType TransformTypeName(cantor::PGTypeName &name);

	//! Transform a list of type modifiers into a list of values
	vector<Value> TransformTypeModifiers(cantor::PGTypeName &name);

	//! Transform a Postgres GROUP BY expression into a list of Expression
	bool TransformGroupBy(optional_ptr<cantor::PGList> group, SelectNode &result);
	void TransformGroupByNode(cantor::PGNode &n, GroupingExpressionMap &map, SelectNode &result,
	                          vector<GroupingSet> &result_sets);
	void AddGroupByExpression(unique_ptr<ParsedExpression> expression, GroupingExpressionMap &map, GroupByNode &result,
	                          vector<idx_t> &result_set);
	void TransformGroupByExpression(cantor::PGNode &n, GroupingExpressionMap &map, GroupByNode &result,
	                                vector<idx_t> &result_set);
	//! Transform a Postgres ORDER BY expression into an OrderByDescription
	bool TransformOrderBy(cantor::PGList *order, vector<OrderByNode> &result);

	//! Transform to a IN or NOT IN expression
	unique_ptr<ParsedExpression> TransformInExpression(const string &name, cantor::PGAExpr &root);

	//! Transform a Postgres SELECT clause into a list of Expressions
	void TransformExpressionList(cantor::PGList &list, vector<unique_ptr<ParsedExpression>> &result);

	//! Transform a Postgres PARTITION BY/ORDER BY specification into lists of expressions
	void TransformWindowDef(cantor::PGWindowDef &window_spec, WindowExpression &expr,
	                        const char *window_name = nullptr);
	//! Transform a Postgres window frame specification into frame expressions
	void TransformWindowFrame(cantor::PGWindowDef &window_spec, WindowExpression &expr);

	unique_ptr<SampleOptions> TransformSampleOptions(optional_ptr<cantor::PGNode> options);
	//! Returns true if an expression is only a star (i.e. "*", without any other decorators)
	bool ExpressionIsEmptyStar(ParsedExpression &expr);

	OnEntryNotFound TransformOnEntryNotFound(bool missing_ok);

	Vector PGListToVector(optional_ptr<cantor::PGList> column_list, idx_t &size);
	vector<string> TransformConflictTarget(cantor::PGList &list);

	unique_ptr<MacroFunction> TransformMacroFunction(cantor::PGFunctionDefinition &function);

	vector<string> TransformNameList(cantor::PGList &list);

public:
	static void SetQueryLocation(ParsedExpression &expr, int query_location);
	static void SetQueryLocation(TableRef &ref, int query_location);

private:
	//! Current stack depth
	idx_t stack_depth;

	void InitializeStackCheck();
	StackChecker<Transformer> StackCheck(idx_t extra_stack = 1);

public:
	template <class T>
	static T &PGCast(cantor::PGNode &node) {
		return reinterpret_cast<T &>(node);
	}
	template <class T>
	static optional_ptr<T> PGPointerCast(void *ptr) {
		return optional_ptr<T>(reinterpret_cast<T *>(ptr));
	}
};

vector<string> ReadPgListToString(cantor::PGList *column_list);

} // namespace goose
