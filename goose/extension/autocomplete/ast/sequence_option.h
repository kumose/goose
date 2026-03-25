#pragma once
#include <goose/parser/qualified_name.h>
#include <goose/parser/parsed_data/create_sequence_info.h>

namespace goose {

class SequenceOption {
public:
	SequenceOption(SequenceInfo type_p) : type(type_p) {
	}

public:
	SequenceInfo type;
};

class ValueSequenceOption : public SequenceOption {
public:
	ValueSequenceOption(SequenceInfo type, Value value_p) : SequenceOption(type), value(value_p) {
	}

public:
	Value value;
};

class QualifiedSequenceOption : public SequenceOption {
public:
	QualifiedSequenceOption(SequenceInfo type, QualifiedName qualified_name_p)
	    : SequenceOption(type), qualified_name(qualified_name_p) {
	}

public:
	QualifiedName qualified_name;
};

} // namespace goose
