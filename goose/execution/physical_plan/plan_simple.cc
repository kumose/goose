#include <goose/execution/operator/helper/physical_load.h>
#include <goose/execution/operator/helper/physical_transaction.h>
#include <goose/execution/operator/helper/physical_update_extensions.h>
#include <goose/execution/operator/helper/physical_vacuum.h>
#include <goose/execution/operator/schema/physical_alter.h>
#include <goose/execution/operator/schema/physical_attach.h>
#include <goose/execution/operator/schema/physical_create_schema.h>
#include <goose/execution/operator/schema/physical_create_view.h>
#include <goose/execution/operator/schema/physical_detach.h>
#include <goose/execution/operator/schema/physical_drop.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/operator/logical_simple.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalSimple &op) {
	switch (op.type) {
	case LogicalOperatorType::LOGICAL_ALTER:
		return Make<PhysicalAlter>(unique_ptr_cast<ParseInfo, AlterInfo>(std::move(op.info)), op.estimated_cardinality);
	case LogicalOperatorType::LOGICAL_DROP:
		return Make<PhysicalDrop>(unique_ptr_cast<ParseInfo, DropInfo>(std::move(op.info)), op.estimated_cardinality);
	case LogicalOperatorType::LOGICAL_TRANSACTION:
		return Make<PhysicalTransaction>(unique_ptr_cast<ParseInfo, TransactionInfo>(std::move(op.info)),
		                                 op.estimated_cardinality);
	case LogicalOperatorType::LOGICAL_LOAD:
		return Make<PhysicalLoad>(unique_ptr_cast<ParseInfo, LoadInfo>(std::move(op.info)), op.estimated_cardinality);
	case LogicalOperatorType::LOGICAL_ATTACH:
		return Make<PhysicalAttach>(unique_ptr_cast<ParseInfo, AttachInfo>(std::move(op.info)),
		                            op.estimated_cardinality);
	case LogicalOperatorType::LOGICAL_DETACH:
		return Make<PhysicalDetach>(unique_ptr_cast<ParseInfo, DetachInfo>(std::move(op.info)),
		                            op.estimated_cardinality);
	case LogicalOperatorType::LOGICAL_UPDATE_EXTENSIONS:
		return Make<PhysicalUpdateExtensions>(unique_ptr_cast<ParseInfo, UpdateExtensionsInfo>(std::move(op.info)),
		                                      op.estimated_cardinality);
	default:
		throw NotImplementedException("Unimplemented type for logical simple operator");
	}
}

} // namespace goose
