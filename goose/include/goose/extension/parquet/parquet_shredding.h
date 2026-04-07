#pragma once

#include <goose/common/serializer/buffered_file_writer.h>
#include <goose/common/case_insensitive_map.h>
#include <goose/common/types/variant.h>

namespace goose {

struct ShreddingType;

struct ChildShreddingTypes {
public:
	ChildShreddingTypes();

public:
	ChildShreddingTypes Copy() const;

public:
	void Serialize(Serializer &serializer) const;
	static ChildShreddingTypes Deserialize(Deserializer &source);

public:
	unique_ptr<case_insensitive_map_t<ShreddingType>> types;
};

struct ShreddingType {
public:
	ShreddingType();
	explicit ShreddingType(const LogicalType &type);

public:
	ShreddingType Copy() const;

public:
	void Serialize(Serializer &serializer) const;
	static ShreddingType Deserialize(Deserializer &source);

public:
	static ShreddingType GetShreddingTypes(const Value &val);
	void AddChild(const string &name, ShreddingType &&child);
	optional_ptr<const ShreddingType> GetChild(const string &name) const;

public:
	bool set = false;
	LogicalType type;
	ChildShreddingTypes children;
};

} // namespace goose
