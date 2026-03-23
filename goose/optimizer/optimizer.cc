#include <goose/optimizer/optimizer.h>

#include <goose/execution/column_binding_resolver.h>
#include <goose/function/function_binder.h>
#include <goose/main/client_context.h>
#include <goose/main/config.h>
#include <goose/main/query_profiler.h>
#include <goose/optimizer/build_probe_side_optimizer.h>
#include <goose/optimizer/column_lifetime_analyzer.h>
#include <goose/optimizer/common_aggregate_optimizer.h>
#include <goose/optimizer/cse_optimizer.h>
#include <goose/optimizer/cte_inlining.h>
#include <goose/optimizer/cte_filter_pusher.h>
#include <goose/optimizer/deliminator.h>
#include <goose/optimizer/empty_result_pullup.h>
#include <goose/optimizer/expression_heuristics.h>
#include <goose/optimizer/filter_pullup.h>
#include <goose/optimizer/filter_pushdown.h>
#include <goose/optimizer/in_clause_rewriter.h>
#include <goose/optimizer/join_elimination.h>
#include <goose/optimizer/join_filter_pushdown_optimizer.h>
#include <goose/optimizer/join_order/join_order_optimizer.h>
#include <goose/optimizer/limit_pushdown.h>
#include <goose/optimizer/regex_range_filter.h>
#include <goose/optimizer/remove_duplicate_groups.h>
#include <goose/optimizer/remove_unused_columns.h>
#include <goose/optimizer/row_group_pruner.h>
#include <goose/optimizer/rule/distinct_aggregate_optimizer.h>
#include <goose/optimizer/rule/equal_or_null_simplification.h>
#include <goose/optimizer/rule/in_clause_simplification.h>
#include <goose/optimizer/rule/join_dependent_filter.h>
#include <goose/optimizer/rule/list.h>
#include <goose/optimizer/sampling_pushdown.h>
#include <goose/optimizer/statistics_propagator.h>
#include <goose/optimizer/sum_rewriter.h>
#include <goose/optimizer/topn_optimizer.h>
#include <goose/optimizer/topn_window_elimination.h>
#include <goose/optimizer/unnest_rewriter.h>
#include <goose/optimizer/late_materialization.h>
#include <goose/optimizer/common_subplan_optimizer.h>
#include <goose/optimizer/window_self_join.h>
#include <goose/planner/binder.h>
#include <goose/planner/planner.h>

namespace goose {

Optimizer::Optimizer(Binder &binder, ClientContext &context) : context(context), binder(binder), rewriter(context) {
	rewriter.rules.push_back(make_uniq<ConstantOrderNormalizationRule>(rewriter));
	rewriter.rules.push_back(make_uniq<ConstantFoldingRule>(rewriter));
	rewriter.rules.push_back(make_uniq<DistributivityRule>(rewriter));
	rewriter.rules.push_back(make_uniq<ArithmeticSimplificationRule>(rewriter));
	rewriter.rules.push_back(make_uniq<CaseSimplificationRule>(rewriter));
	rewriter.rules.push_back(make_uniq<ConjunctionSimplificationRule>(rewriter));
	rewriter.rules.push_back(make_uniq<DatePartSimplificationRule>(rewriter));
	rewriter.rules.push_back(make_uniq<DateTruncSimplificationRule>(rewriter));
	rewriter.rules.push_back(make_uniq<ComparisonSimplificationRule>(rewriter));
	rewriter.rules.push_back(make_uniq<InClauseSimplificationRule>(rewriter));
	rewriter.rules.push_back(make_uniq<EqualOrNullSimplification>(rewriter));
	rewriter.rules.push_back(make_uniq<MoveConstantsRule>(rewriter));
	rewriter.rules.push_back(make_uniq<LikeOptimizationRule>(rewriter));
	rewriter.rules.push_back(make_uniq<OrderedAggregateOptimizer>(rewriter));
	rewriter.rules.push_back(make_uniq<DistinctAggregateOptimizer>(rewriter));
	rewriter.rules.push_back(make_uniq<DistinctWindowedOptimizer>(rewriter));
	rewriter.rules.push_back(make_uniq<RegexOptimizationRule>(rewriter));
	rewriter.rules.push_back(make_uniq<EmptyNeedleRemovalRule>(rewriter));
	rewriter.rules.push_back(make_uniq<EnumComparisonRule>(rewriter));
	rewriter.rules.push_back(make_uniq<JoinDependentFilterRule>(rewriter));
	rewriter.rules.push_back(make_uniq<TimeStampComparison>(context, rewriter));

#ifdef DEBUG
	for (auto &rule : rewriter.rules) {
		// root not defined in rule
		D_ASSERT(rule->root);
	}
#endif
}

ClientContext &Optimizer::GetContext() {
	return context;
}

bool Optimizer::OptimizerDisabled(OptimizerType type) {
	return OptimizerDisabled(context, type);
}

bool Optimizer::OptimizerDisabled(ClientContext &context_p, OptimizerType type) {
	auto &config = DBConfig::GetConfig(context_p);
	return config.options.disabled_optimizers.find(type) != config.options.disabled_optimizers.end();
}

void Optimizer::RunOptimizer(OptimizerType type, const std::function<void()> &callback) {
	if (OptimizerDisabled(type)) {
		// optimizer is marked as disabled: skip
		return;
	}
	auto &profiler = QueryProfiler::Get(context);
	profiler.StartPhase(MetricsUtils::GetOptimizerMetricByType(type));
	callback();
	profiler.EndPhase();
	if (plan) {
		Verify(*plan);
	}
}

void Optimizer::Verify(LogicalOperator &op) {
	ColumnBindingResolver::Verify(op);
}

void Optimizer::RunBuiltInOptimizers() {
	switch (plan->type) {
	case LogicalOperatorType::LOGICAL_TRANSACTION:
	case LogicalOperatorType::LOGICAL_PRAGMA:
	case LogicalOperatorType::LOGICAL_SET:
	case LogicalOperatorType::LOGICAL_ATTACH:
	case LogicalOperatorType::LOGICAL_UPDATE_EXTENSIONS:
	case LogicalOperatorType::LOGICAL_CREATE_SECRET:
	case LogicalOperatorType::LOGICAL_EXTENSION_OPERATOR:
		// skip optimizing simple & often-occurring plans unaffected by rewrites
		if (plan->children.empty()) {
			return;
		}
		break;
	default:
		break;
	}
	// first we perform expression rewrites using the ExpressionRewriter
	// this does not change the logical plan structure, but only simplifies the expression trees
	RunOptimizer(OptimizerType::EXPRESSION_REWRITER, [&]() { rewriter.VisitOperator(*plan); });

	// try to inline CTEs instead of materialization
	RunOptimizer(OptimizerType::CTE_INLINING, [&]() {
		CTEInlining cte_inlining(*this);
		plan = cte_inlining.Optimize(std::move(plan));
	});

	// Rewrites SUM(x + C) into SUM(x) + C * COUNT(x)
	RunOptimizer(OptimizerType::SUM_REWRITER, [&]() {
		SumRewriterOptimizer optimizer(*this);
		optimizer.Optimize(plan);
	});

	// perform filter pullup
	RunOptimizer(OptimizerType::FILTER_PULLUP, [&]() {
		FilterPullup filter_pullup;
		plan = filter_pullup.Rewrite(std::move(plan));
	});

	// perform filter pushdown
	RunOptimizer(OptimizerType::FILTER_PUSHDOWN, [&]() {
		FilterPushdown filter_pushdown(*this);
		unordered_set<idx_t> top_bindings;
		filter_pushdown.CheckMarkToSemi(*plan, top_bindings);
		plan = filter_pushdown.Rewrite(std::move(plan));
	});

	// derive and push filters into materialized CTEs
	RunOptimizer(OptimizerType::CTE_FILTER_PUSHER, [&]() {
		CTEFilterPusher cte_filter_pusher(*this);
		plan = cte_filter_pusher.Optimize(std::move(plan));
	});

	RunOptimizer(OptimizerType::REGEX_RANGE, [&]() {
		RegexRangeFilter regex_opt;
		plan = regex_opt.Rewrite(std::move(plan));
	});

	RunOptimizer(OptimizerType::IN_CLAUSE, [&]() {
		InClauseRewriter ic_rewriter(context, *this);
		plan = ic_rewriter.Rewrite(std::move(plan));
	});

	// removes any redundant DelimGets/DelimJoins
	RunOptimizer(OptimizerType::DELIMINATOR, [&]() {
		Deliminator deliminator;
		plan = deliminator.Optimize(std::move(plan));
	});

	// try to inline CTEs instead of materialization
	RunOptimizer(OptimizerType::CTE_INLINING, [&]() {
		CTEInlining cte_inlining(*this);
		plan = cte_inlining.Optimize(std::move(plan));
	});

	// Pulls up empty results
	RunOptimizer(OptimizerType::EMPTY_RESULT_PULLUP, [&]() {
		EmptyResultPullup empty_result_pullup;
		plan = empty_result_pullup.Optimize(std::move(plan));
	});

	// Replaces some window computations with self-joins
	RunOptimizer(OptimizerType::WINDOW_SELF_JOIN, [&]() {
		WindowSelfJoinOptimizer window_self_join_optimizer(*this);
		plan = window_self_join_optimizer.Optimize(std::move(plan));
	});

	// then we perform the join ordering optimization
	// this also rewrites cross products + filters into joins and performs filter pushdowns
	RunOptimizer(OptimizerType::JOIN_ORDER, [&]() {
		JoinOrderOptimizer optimizer(context);
		plan = optimizer.Optimize(std::move(plan));
	});

	RunOptimizer(OptimizerType::JOIN_ELIMINATION, [&]() {
		JoinElimination join_elimination;
		plan = join_elimination.Optimize(std::move(plan));
	});

	// rewrites UNNESTs in DelimJoins by moving them to the projection
	RunOptimizer(OptimizerType::UNNEST_REWRITER, [&]() {
		UnnestRewriter unnest_rewriter;
		plan = unnest_rewriter.Optimize(std::move(plan));
	});

	// removes unused columns
	RunOptimizer(OptimizerType::UNUSED_COLUMNS, [&]() {
		RemoveUnusedColumns unused(binder, context, true);
		unused.VisitOperator(*plan);
	});

	// Remove duplicate groups from aggregates
	RunOptimizer(OptimizerType::DUPLICATE_GROUPS, [&]() {
		RemoveDuplicateGroups remove;
		remove.VisitOperator(*plan);
	});

	// then we extract common subexpressions inside the different operators
	RunOptimizer(OptimizerType::COMMON_SUBEXPRESSIONS, [&]() {
		CommonSubExpressionOptimizer cse_optimizer(binder);
		cse_optimizer.VisitOperator(*plan);
	});

	// creates projection maps so unused columns are projected out early
	RunOptimizer(OptimizerType::COLUMN_LIFETIME, [&]() {
		ColumnLifetimeAnalyzer column_lifetime(*this, *plan, true);
		column_lifetime.VisitOperator(*plan);
	});

	// Once we know the column lifetime, we have more information regarding
	// what relations should be the build side/probe side.
	RunOptimizer(OptimizerType::BUILD_SIDE_PROBE_SIDE, [&]() {
		BuildProbeSideOptimizer build_probe_side_optimizer(context, *plan);
		build_probe_side_optimizer.VisitOperator(*plan);
	});

	// convert common subplans into materialized CTEs
	RunOptimizer(OptimizerType::COMMON_SUBPLAN, [&]() {
		CommonSubplanOptimizer common_subplan_optimizer(*this);
		plan = common_subplan_optimizer.Optimize(std::move(plan));
	});

	// pushes LIMIT below PROJECTION
	RunOptimizer(OptimizerType::LIMIT_PUSHDOWN, [&]() {
		LimitPushdown limit_pushdown;
		plan = limit_pushdown.Optimize(std::move(plan));
	});

	RunOptimizer(OptimizerType::ROW_GROUP_PRUNER, [&]() {
		RowGroupPruner row_group_pruner(context);
		plan = row_group_pruner.Optimize(std::move(plan));
	});

	// perform sampling pushdown
	RunOptimizer(OptimizerType::SAMPLING_PUSHDOWN, [&]() {
		SamplingPushdown sampling_pushdown;
		plan = sampling_pushdown.Optimize(std::move(plan));
	});

	// transform ORDER BY + LIMIT to TopN
	RunOptimizer(OptimizerType::TOP_N, [&]() {
		TopN topn(context);
		plan = topn.Optimize(std::move(plan));
	});

	// try to use late materialization
	RunOptimizer(OptimizerType::LATE_MATERIALIZATION, [&]() {
		LateMaterialization late_materialization(*this);
		plan = late_materialization.Optimize(std::move(plan));
	});

	// perform statistics propagation
	column_binding_map_t<unique_ptr<BaseStatistics>> statistics_map;
	RunOptimizer(OptimizerType::STATISTICS_PROPAGATION, [&]() {
		StatisticsPropagator propagator(*this, *plan);
		propagator.PropagateStatistics(plan);
		statistics_map = propagator.GetStatisticsMap();
	});

	// rewrite row_number window function + filter on row_number to aggregate
	RunOptimizer(OptimizerType::TOP_N_WINDOW_ELIMINATION, [&]() {
		TopNWindowElimination topn_window_elimination(context, *this, &statistics_map);
		plan = topn_window_elimination.Optimize(std::move(plan));
	});

	// remove duplicate aggregates
	RunOptimizer(OptimizerType::COMMON_AGGREGATE, [&]() {
		CommonAggregateOptimizer common_aggregate;
		common_aggregate.VisitOperator(*plan);
	});

	// creates projection maps so unused columns are projected out early
	RunOptimizer(OptimizerType::COLUMN_LIFETIME, [&]() {
		ColumnLifetimeAnalyzer column_lifetime(*this, *plan, true);
		column_lifetime.VisitOperator(*plan);
	});

	// apply simple expression heuristics to get an initial reordering
	RunOptimizer(OptimizerType::REORDER_FILTER, [&]() {
		ExpressionHeuristics expression_heuristics(*this);
		plan = expression_heuristics.Rewrite(std::move(plan));
	});

	// perform join filter pushdown after the dust has settled
	RunOptimizer(OptimizerType::JOIN_FILTER_PUSHDOWN, [&]() {
		JoinFilterPushdownOptimizer join_filter_pushdown(*this);
		join_filter_pushdown.VisitOperator(*plan);
	});
}

unique_ptr<LogicalOperator> Optimizer::Optimize(unique_ptr<LogicalOperator> plan_p) {
	Verify(*plan_p);

	this->plan = std::move(plan_p);

	for (auto &pre_optimizer_extension : DBConfig::GetConfig(context).optimizer_extensions) {
		RunOptimizer(OptimizerType::EXTENSION, [&]() {
			OptimizerExtensionInput input {GetContext(), *this, pre_optimizer_extension.optimizer_info.get()};
			if (pre_optimizer_extension.pre_optimize_function) {
				pre_optimizer_extension.pre_optimize_function(input, plan);
			}
		});
	}

	RunBuiltInOptimizers();

	for (auto &optimizer_extension : DBConfig::GetConfig(context).optimizer_extensions) {
		RunOptimizer(OptimizerType::EXTENSION, [&]() {
			OptimizerExtensionInput input {GetContext(), *this, optimizer_extension.optimizer_info.get()};
			if (optimizer_extension.optimize_function) {
				optimizer_extension.optimize_function(input, plan);
			}
		});
	}

	Planner::VerifyPlan(context, plan);

	return std::move(plan);
}

unique_ptr<Expression> Optimizer::BindScalarFunction(const string &name, unique_ptr<Expression> c1) {
	vector<unique_ptr<Expression>> children;
	children.push_back(std::move(c1));
	return BindScalarFunction(name, std::move(children));
}

unique_ptr<Expression> Optimizer::BindScalarFunction(const string &name, unique_ptr<Expression> c1,
                                                     unique_ptr<Expression> c2) {
	vector<unique_ptr<Expression>> children;
	children.push_back(std::move(c1));
	children.push_back(std::move(c2));
	return BindScalarFunction(name, std::move(children));
}

unique_ptr<Expression> Optimizer::BindScalarFunction(const string &name, vector<unique_ptr<Expression>> children) {
	FunctionBinder binder(context);
	ErrorData error;
	auto expr = binder.BindScalarFunction(DEFAULT_SCHEMA, name, std::move(children), error);
	if (error.HasError()) {
		throw InternalException("Optimizer exception - failed to bind function %s: %s", name, error.Message());
	}
	return expr;
}

} // namespace goose
