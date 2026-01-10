#pragma once

#include <goose/main/relation/table_function_relation.h>
#include <goose/main/client_context.h>
#include <goose/common/named_parameter_map.h>
#include <goose/parser/column_definition.h>
#include <goose/common/string.h>
#include <goose/common/vector.h>

namespace goose {

class ReadJSONRelation : public TableFunctionRelation {
public:
	ReadJSONRelation(const shared_ptr<ClientContext> &context, string json_file, named_parameter_map_t options,
	                 bool auto_detect, string alias = "");
	ReadJSONRelation(const shared_ptr<ClientContext> &context, vector<string> &json_file, named_parameter_map_t options,
	                 bool auto_detect, string alias = "");
	~ReadJSONRelation() override;
	string json_file;
	string alias;

public:
	string GetAlias() override;

private:
	void InitializeAlias(const vector<string> &input);
};

} // namespace goose
