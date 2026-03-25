#pragma once

#include <goose/common/serializer/buffered_file_writer.h>
#include <goose/common/case_insensitive_map.h>

namespace goose {

struct FieldID;
struct ChildFieldIDs {
	ChildFieldIDs();
	ChildFieldIDs Copy() const;
	unique_ptr<case_insensitive_map_t<FieldID>> ids;

	void Serialize(Serializer &serializer) const;
	static ChildFieldIDs Deserialize(Deserializer &source);
};

struct FieldID {
public:
	static constexpr const auto GOOSE_FIELD_ID = "__goose_field_id";
	FieldID();
	explicit FieldID(int32_t field_id);
	FieldID Copy() const;
	bool set;
	int32_t field_id;
	ChildFieldIDs child_field_ids;

	void Serialize(Serializer &serializer) const;
	static FieldID Deserialize(Deserializer &source);

public:
	static void GenerateFieldIDs(ChildFieldIDs &field_ids, idx_t &field_id, const vector<string> &names,
	                             const vector<LogicalType> &sql_types);
	static void GetFieldIDs(const Value &field_ids_value, ChildFieldIDs &field_ids,
	                        unordered_set<uint32_t> &unique_field_ids,
	                        const case_insensitive_map_t<LogicalType> &name_to_type_map);
};

} // namespace goose
