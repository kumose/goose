#include <goose/common/tree_renderer/graphviz_tree_renderer.h>

#include <goose/common/types-import.h>
#include <goose/common/string_util.h>
#include <goose/execution/operator/aggregate/physical_hash_aggregate.h>
#include <goose/execution/operator/join/physical_delim_join.h>
#include <goose/execution/operator/scan/physical_positional_scan.h>
#include <goose/execution/physical_operator.h>
#include <goose/parallel/pipeline.h>
#include <goose/planner/logical_operator.h>
#include <goose/main/query_profiler.h>
#include <goose/utility/utf8proc_wrapper.h>

#include <sstream>

namespace goose {

string GRAPHVIZTreeRenderer::ToString(const LogicalOperator &op) {
	goose::stringstream ss;
	Render(op, ss);
	return ss.str();
}

string GRAPHVIZTreeRenderer::ToString(const PhysicalOperator &op) {
	goose::stringstream ss;
	Render(op, ss);
	return ss.str();
}

string GRAPHVIZTreeRenderer::ToString(const ProfilingNode &op) {
	goose::stringstream ss;
	Render(op, ss);
	return ss.str();
}

string GRAPHVIZTreeRenderer::ToString(const Pipeline &op) {
	goose::stringstream ss;
	Render(op, ss);
	return ss.str();
}

void GRAPHVIZTreeRenderer::Render(const LogicalOperator &op, std::ostream &ss) {
	auto tree = RenderTree::CreateRenderTree(op);
	ToStream(*tree, ss);
}

void GRAPHVIZTreeRenderer::Render(const PhysicalOperator &op, std::ostream &ss) {
	auto tree = RenderTree::CreateRenderTree(op);
	ToStream(*tree, ss);
}

void GRAPHVIZTreeRenderer::Render(const ProfilingNode &op, std::ostream &ss) {
	auto tree = RenderTree::CreateRenderTree(op);
	ToStream(*tree, ss);
}

void GRAPHVIZTreeRenderer::Render(const Pipeline &op, std::ostream &ss) {
	auto tree = RenderTree::CreateRenderTree(op);
	ToStream(*tree, ss);
}

void GRAPHVIZTreeRenderer::ToStreamInternal(RenderTree &root, std::ostream &ss) {
	const string digraph_format = R"(
digraph G {
    node [shape=box, style=rounded, fontname="Courier New", fontsize=10];
%s
%s
}
	)";

	vector<string> nodes;
	vector<string> edges;

	const string node_format = R"(    node_%d_%d [label="%s"];)";

	for (idx_t y = 0; y < root.height; y++) {
		for (idx_t x = 0; x < root.width; x++) {
			auto node = root.GetNode(x, y);
			if (!node) {
				continue;
			}

			// Create Node
			vector<string> body;
			body.push_back(node->name);
			for (auto &item : node->extra_text) {
				auto &key = item.first;
				auto &value_raw = item.second;

				auto value = QueryProfiler::JSONSanitize(value_raw);
				body.push_back(StringUtil::Format("%s:\\n%s", key, value));
			}
			nodes.push_back(StringUtil::Format(node_format, x, y, StringUtil::Join(body, "\\n───\\n")));

			// Create Edge(s)
			for (auto &coord : node->child_positions) {
				edges.push_back(StringUtil::Format("    node_%d_%d -> node_%d_%d;", x, y, coord.x, coord.y));
			}
		}
	}
	auto node_lines = StringUtil::Join(nodes, "\n");
	auto edge_lines = StringUtil::Join(edges, "\n");

	string result = StringUtil::Format(digraph_format, node_lines, edge_lines);
	ss << result;
}

} // namespace goose
