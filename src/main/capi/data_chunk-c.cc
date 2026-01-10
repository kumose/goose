#include <goose/common/types/data_chunk.h>
#include <goose/common/types/string_type.h>
#include <goose/main/capi/capi_internal.h>
#include <goose/common/type_visitor.h>

#include <string.h>

goose_data_chunk goose_create_data_chunk(goose_logical_type *column_types, idx_t column_count) {
	if (!column_types) {
		return nullptr;
	}
	goose::vector<goose::LogicalType> types;
	for (idx_t i = 0; i < column_count; i++) {
		auto logical_type = reinterpret_cast<goose::LogicalType *>(column_types[i]);
		if (goose::TypeVisitor::Contains(*logical_type, goose::LogicalTypeId::INVALID) ||
		    goose::TypeVisitor::Contains(*logical_type, goose::LogicalTypeId::ANY)) {
			return nullptr;
		}
		types.push_back(*logical_type);
	}

	auto result = new goose::DataChunk();
	try {
		result->Initialize(goose::Allocator::DefaultAllocator(), types);
	} catch (...) {
		delete result;
		return nullptr;
	}

	return reinterpret_cast<goose_data_chunk>(result);
}

void goose_destroy_data_chunk(goose_data_chunk *chunk) {
	if (chunk && *chunk) {
		auto data_chunk = reinterpret_cast<goose::DataChunk *>(*chunk);
		delete data_chunk;
		*chunk = nullptr;
	}
}

void goose_data_chunk_reset(goose_data_chunk chunk) {
	if (!chunk) {
		return;
	}
	auto dchunk = reinterpret_cast<goose::DataChunk *>(chunk);
	dchunk->Reset();
}

goose_vector goose_create_vector(goose_logical_type type, idx_t capacity) {
	auto dtype = reinterpret_cast<goose::LogicalType *>(type);
	try {
		auto vector = new goose::Vector(*dtype, capacity);
		return reinterpret_cast<goose_vector>(vector);
	} catch (...) {
		return nullptr;
	}
}

void goose_destroy_vector(goose_vector *vector) {
	if (vector && *vector) {
		auto dvector = reinterpret_cast<goose::Vector *>(*vector);
		delete dvector;
		*vector = nullptr;
	}
}

idx_t goose_data_chunk_get_column_count(goose_data_chunk chunk) {
	if (!chunk) {
		return 0;
	}
	auto dchunk = reinterpret_cast<goose::DataChunk *>(chunk);
	return dchunk->ColumnCount();
}

goose_vector goose_data_chunk_get_vector(goose_data_chunk chunk, idx_t col_idx) {
	if (!chunk || col_idx >= goose_data_chunk_get_column_count(chunk)) {
		return nullptr;
	}
	auto dchunk = reinterpret_cast<goose::DataChunk *>(chunk);
	return reinterpret_cast<goose_vector>(&dchunk->data[col_idx]);
}

idx_t goose_data_chunk_get_size(goose_data_chunk chunk) {
	if (!chunk) {
		return 0;
	}
	auto dchunk = reinterpret_cast<goose::DataChunk *>(chunk);
	return dchunk->size();
}

void goose_data_chunk_set_size(goose_data_chunk chunk, idx_t size) {
	if (!chunk) {
		return;
	}
	auto dchunk = reinterpret_cast<goose::DataChunk *>(chunk);
	dchunk->SetCardinality(size);
}

goose_logical_type goose_vector_get_column_type(goose_vector vector) {
	if (!vector) {
		return nullptr;
	}
	auto v = reinterpret_cast<goose::Vector *>(vector);
	return reinterpret_cast<goose_logical_type>(new goose::LogicalType(v->GetType()));
}

void *goose_vector_get_data(goose_vector vector) {
	if (!vector) {
		return nullptr;
	}
	auto v = reinterpret_cast<goose::Vector *>(vector);
	return goose::FlatVector::GetData(*v);
}

uint64_t *goose_vector_get_validity(goose_vector vector) {
	if (!vector) {
		return nullptr;
	}
	auto v = reinterpret_cast<goose::Vector *>(vector);
	switch (v->GetVectorType()) {
	case goose::VectorType::CONSTANT_VECTOR:
		return goose::ConstantVector::Validity(*v).GetData();
	case goose::VectorType::FLAT_VECTOR:
		return goose::FlatVector::Validity(*v).GetData();
	default:
		return nullptr;
	}
}

void goose_vector_ensure_validity_writable(goose_vector vector) {
	if (!vector) {
		return;
	}
	auto v = reinterpret_cast<goose::Vector *>(vector);
	auto &validity = goose::FlatVector::Validity(*v);
	validity.EnsureWritable();
}

void goose_vector_assign_string_element(goose_vector vector, idx_t index, const char *str) {
	goose_vector_assign_string_element_len(vector, index, str, strlen(str));
}

void goose_vector_assign_string_element_len(goose_vector vector, idx_t index, const char *str, idx_t str_len) {
	if (!vector) {
		return;
	}
	auto v = reinterpret_cast<goose::Vector *>(vector);
	auto data = goose::FlatVector::GetData<goose::string_t>(*v);
	data[index] = goose::StringVector::AddStringOrBlob(*v, str, str_len);
}

goose_vector goose_list_vector_get_child(goose_vector vector) {
	if (!vector) {
		return nullptr;
	}
	auto v = reinterpret_cast<goose::Vector *>(vector);
	return reinterpret_cast<goose_vector>(&goose::ListVector::GetEntry(*v));
}

idx_t goose_list_vector_get_size(goose_vector vector) {
	if (!vector) {
		return 0;
	}
	auto v = reinterpret_cast<goose::Vector *>(vector);
	return goose::ListVector::GetListSize(*v);
}

goose_state goose_list_vector_set_size(goose_vector vector, idx_t size) {
	if (!vector) {
		return GooseError;
	}
	auto v = reinterpret_cast<goose::Vector *>(vector);
	goose::ListVector::SetListSize(*v, size);
	return GooseSuccess;
}

goose_state goose_list_vector_reserve(goose_vector vector, idx_t required_capacity) {
	if (!vector) {
		return GooseError;
	}
	auto v = reinterpret_cast<goose::Vector *>(vector);
	goose::ListVector::Reserve(*v, required_capacity);
	return GooseSuccess;
}

goose_vector goose_struct_vector_get_child(goose_vector vector, idx_t index) {
	if (!vector) {
		return nullptr;
	}
	auto v = reinterpret_cast<goose::Vector *>(vector);
	return reinterpret_cast<goose_vector>(goose::StructVector::GetEntries(*v)[index].get());
}

goose_vector goose_array_vector_get_child(goose_vector vector) {
	if (!vector) {
		return nullptr;
	}
	auto v = reinterpret_cast<goose::Vector *>(vector);
	return reinterpret_cast<goose_vector>(&goose::ArrayVector::GetEntry(*v));
}

bool goose_validity_row_is_valid(uint64_t *validity, idx_t row) {
	if (!validity) {
		return true;
	}
	idx_t entry_idx = row / 64;
	idx_t idx_in_entry = row % 64;
	return validity[entry_idx] & ((idx_t)1 << idx_in_entry);
}

void goose_validity_set_row_validity(uint64_t *validity, idx_t row, bool valid) {
	if (valid) {
		goose_validity_set_row_valid(validity, row);
	} else {
		goose_validity_set_row_invalid(validity, row);
	}
}

void goose_validity_set_row_invalid(uint64_t *validity, idx_t row) {
	if (!validity) {
		return;
	}
	idx_t entry_idx = row / 64;
	idx_t idx_in_entry = row % 64;
	validity[entry_idx] &= ~((uint64_t)1 << idx_in_entry);
}

void goose_validity_set_row_valid(uint64_t *validity, idx_t row) {
	if (!validity) {
		return;
	}
	idx_t entry_idx = row / 64;
	idx_t idx_in_entry = row % 64;
	validity[entry_idx] |= (uint64_t)1 << idx_in_entry;
}

goose_selection_vector goose_create_selection_vector(idx_t size) {
	return reinterpret_cast<goose_selection_vector>(new goose::SelectionVector(size));
}

void goose_destroy_selection_vector(goose_selection_vector sel) {
	delete reinterpret_cast<goose::SelectionVector *>(sel);
}

sel_t *goose_selection_vector_get_data_ptr(goose_selection_vector sel) {
	return reinterpret_cast<goose::SelectionVector *>(sel)->data();
}

void goose_slice_vector(goose_vector dict, goose_selection_vector sel, idx_t len) {
	auto d_dict = reinterpret_cast<goose::Vector *>(dict);
	auto d_sel = reinterpret_cast<goose::SelectionVector *>(sel);
	d_dict->Slice(*d_sel, len);
}

void goose_vector_copy_sel(goose_vector src, goose_vector dst, goose_selection_vector sel, idx_t src_count,
                            idx_t src_offset, idx_t dst_offset) {
	auto d_src = reinterpret_cast<goose::Vector *>(src);
	auto d_dst = reinterpret_cast<goose::Vector *>(dst);
	auto d_sel = reinterpret_cast<goose::SelectionVector *>(sel);
	goose::VectorOperations::Copy(*d_src, *d_dst, *d_sel, src_count, src_offset, dst_offset);
}

void goose_vector_reference_value(goose_vector vector, goose_value value) {
	auto dvector = reinterpret_cast<goose::Vector *>(vector);
	auto dvalue = reinterpret_cast<goose::Value *>(value);
	dvector->Reference(*dvalue);
}

void goose_vector_reference_vector(goose_vector to_vector, goose_vector from_vector) {
	auto dto_vector = reinterpret_cast<goose::Vector *>(to_vector);
	auto dfrom_vector = reinterpret_cast<goose::Vector *>(from_vector);
	dto_vector->Reference(*dfrom_vector);
}
