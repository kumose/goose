#include <goose/planner/operator/logical_extension_operator.h>
#include <goose/execution/column_binding_resolver.h>
#include <goose/main/config.h>
#include <goose/common/serializer/serializer.h>
#include <goose/common/serializer/deserializer.h>

namespace goose {

void LogicalExtensionOperator::ResolveColumnBindings(ColumnBindingResolver &res, vector<ColumnBinding> &bindings) {
	// general case
	// first visit the children of this operator
	for (auto &child : children) {
		res.VisitOperator(*child);
	}
	// now visit the expressions of this operator to resolve any bound column references
	for (auto &expression : expressions) {
		res.VisitExpression(&expression);
	}
	// finally update the current set of bindings to the current set of column bindings
	bindings = GetColumnBindings();
}

void LogicalExtensionOperator::Serialize(Serializer &serializer) const {
	LogicalOperator::Serialize(serializer);
	serializer.WriteProperty(200, "extension_name", GetExtensionName());
}

unique_ptr<LogicalOperator> LogicalExtensionOperator::Deserialize(Deserializer &deserializer) {
	auto &config = DBConfig::GetConfig(deserializer.Get<ClientContext &>());
	auto extension_name = deserializer.ReadProperty<string>(200, "extension_name");
	for (auto &extension : config.operator_extensions) {
		if (extension->GetName() == extension_name) {
			return extension->Deserialize(deserializer);
		}
	}
	throw SerializationException("No deserialization method exists for extension: " + extension_name);
}

string LogicalExtensionOperator::GetExtensionName() const {
	throw SerializationException("LogicalExtensionOperator::GetExtensionName not implemented which is required for "
	                             "serializing extension operators");
}

} // namespace goose
