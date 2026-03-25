#include <goose/optimizer/join_order/join_node.h>

#include <goose/common/limits.h>
#include <goose/planner/expression/list.h>
#include <goose/planner/operator/list.h>

namespace goose {

DPJoinNode::DPJoinNode(JoinRelationSet &set) : set(set), info(nullptr), is_leaf(true), left_set(set), right_set(set) {
}

DPJoinNode::DPJoinNode(JoinRelationSet &set, optional_ptr<NeighborInfo> info, JoinRelationSet &left,
                       JoinRelationSet &right, double cost)
    : set(set), info(info), is_leaf(false), left_set(left), right_set(right), cost(cost) {
}

} // namespace goose
