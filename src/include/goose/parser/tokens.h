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

namespace goose {

//===--------------------------------------------------------------------===//
// Statements
//===--------------------------------------------------------------------===//
class SQLStatement;

class AlterStatement;
class AttachStatement;
class CallStatement;
class CopyStatement;
class CreateStatement;
class DetachStatement;
class DeleteStatement;
class DropStatement;
class ExtensionStatement;
class InsertStatement;
class SelectStatement;
class TransactionStatement;
class UpdateStatement;
class UpdateExtensionsStatement;
class PrepareStatement;
class ExecuteStatement;
class PragmaStatement;
class ExplainStatement;
class ExportStatement;
class VacuumStatement;
class RelationStatement;
class SetStatement;
class SetVariableStatement;
class ResetVariableStatement;
class LoadStatement;
class LogicalPlanStatement;
class MultiStatement;
class CopyDatabaseStatement;
class MergeIntoStatement;

//===--------------------------------------------------------------------===//
// Query Node
//===--------------------------------------------------------------------===//
class QueryNode;
class SelectNode;
class SetOperationNode;
class RecursiveCTENode;
class CTENode;
class StatementNode;

//===--------------------------------------------------------------------===//
// Expressions
//===--------------------------------------------------------------------===//
class ParsedExpression;

class BetweenExpression;
class CaseExpression;
class CastExpression;
class CollateExpression;
class ColumnRefExpression;
class ComparisonExpression;
class ConjunctionExpression;
class ConstantExpression;
class DefaultExpression;
class FunctionExpression;
class LambdaExpression;
class OperatorExpression;
class ParameterExpression;
class PositionalReferenceExpression;
class StarExpression;
class SubqueryExpression;
class WindowExpression;

//===--------------------------------------------------------------------===//
// Constraints
//===--------------------------------------------------------------------===//
class Constraint;

class NotNullConstraint;
class CheckConstraint;
class UniqueConstraint;
class ForeignKeyConstraint;

//===--------------------------------------------------------------------===//
// TableRefs
//===--------------------------------------------------------------------===//
class TableRef;

class BaseTableRef;
class BoundRefWrapper;
class JoinRef;
class SubqueryRef;
class TableFunctionRef;
class EmptyTableRef;
class ExpressionListRef;
class ColumnDataRef;
class PivotRef;
class ShowRef;

//===--------------------------------------------------------------------===//
// Other
//===--------------------------------------------------------------------===//
class SampleOptions;
class MergeIntoAction;

} // namespace goose
