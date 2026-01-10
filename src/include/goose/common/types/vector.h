// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <goose/common/types-import.h>
#include <goose/common/common.h>
#include <goose/common/enums/vector_type.h>
#include <goose/common/types-import.h>
#include <goose/common/types/selection_vector.h>
#include <goose/common/types/validity_mask.h>
#include <goose/common/types/value.h>
#include <goose/common/types/vector_buffer.h>
#include <goose/common/vector_size.h>
#include <goose/common/type_util.h>

namespace goose {

class VectorCache;
class VectorChildBuffer;
class VectorStringBuffer;
class VectorStructBuffer;
class VectorListBuffer;
struct SelCache;

struct UnifiedVectorFormat {
	GOOSE_API UnifiedVectorFormat();
	// disable copy constructors
	UnifiedVectorFormat(const UnifiedVectorFormat &other) = delete;
	UnifiedVectorFormat &operator=(const UnifiedVectorFormat &) = delete;
	//! enable move constructors
	GOOSE_API UnifiedVectorFormat(UnifiedVectorFormat &&other) noexcept;
	GOOSE_API UnifiedVectorFormat &operator=(UnifiedVectorFormat &&) noexcept;

	const SelectionVector *sel;
	data_ptr_t data;
	ValidityMask validity;
	SelectionVector owned_sel;
	PhysicalType physical_type;

	template <class T>
	void VerifyVectorType() const {
#ifdef GOOSE_DEBUG_NO_SAFETY
		D_ASSERT(StorageTypeCompatible<T>(physical_type));
#else
		if (!StorageTypeCompatible<T>(physical_type)) {
			throw InternalException("Expected unified vector format of type %s, but found type %s", GetTypeId<T>(),
			                        physical_type);
		}
#endif
	}

	template <class T>
	static inline const T *GetDataUnsafe(const UnifiedVectorFormat &format) {
		return reinterpret_cast<const T *>(format.data);
	}
	template <class T>
	static inline const T *GetData(const UnifiedVectorFormat &format) {
		return format.GetData<T>();
	}
	template <class T>
	inline const T *GetData() const {
		VerifyVectorType<T>();
		return GetDataUnsafe<T>(*this);
	}
	template <class T>
	static inline T *GetDataNoConst(UnifiedVectorFormat &format) {
		format.VerifyVectorType<T>();
		return reinterpret_cast<T *>(format.data);
	}
};

struct RecursiveUnifiedVectorFormat {
	UnifiedVectorFormat unified;
	vector<RecursiveUnifiedVectorFormat> children;
	LogicalType logical_type;
};

struct UnifiedVariantVector {
	//! The 'keys' list (dictionary)
	GOOSE_API static const UnifiedVectorFormat &GetKeys(const RecursiveUnifiedVectorFormat &vec);
	//! The 'keys' list entry
	GOOSE_API static const UnifiedVectorFormat &GetKeysEntry(const RecursiveUnifiedVectorFormat &vec);
	//! The 'children' list
	GOOSE_API static const UnifiedVectorFormat &GetChildren(const RecursiveUnifiedVectorFormat &vec);
	//! The 'keys_index' inside the 'children' list
	GOOSE_API static const UnifiedVectorFormat &GetChildrenKeysIndex(const RecursiveUnifiedVectorFormat &vec);
	//! The 'values_index' inside the 'children' list
	GOOSE_API static const UnifiedVectorFormat &GetChildrenValuesIndex(const RecursiveUnifiedVectorFormat &vec);
	//! The 'values' list
	GOOSE_API static const UnifiedVectorFormat &GetValues(const RecursiveUnifiedVectorFormat &vec);
	//! The 'type_id' inside the 'values' list
	GOOSE_API static const UnifiedVectorFormat &GetValuesTypeId(const RecursiveUnifiedVectorFormat &vec);
	//! The 'byte_offset' inside the 'values' list
	GOOSE_API static const UnifiedVectorFormat &GetValuesByteOffset(const RecursiveUnifiedVectorFormat &vec);
	//! The binary blob 'data' encoding the Variant for the row
	GOOSE_API static const UnifiedVectorFormat &GetData(const RecursiveUnifiedVectorFormat &vec);
};

//! This is a helper data structure. It contains all fields necessary to resize a vector.
struct ResizeInfo {
	ResizeInfo(Vector &vec, data_ptr_t data, optional_ptr<VectorBuffer> buffer, const idx_t multiplier)
	    : vec(vec), data(data), buffer(buffer), multiplier(multiplier) {
	}

	Vector &vec;
	data_ptr_t data;
	optional_ptr<VectorBuffer> buffer;
	idx_t multiplier;
};

struct ConsecutiveChildListInfo {
	ConsecutiveChildListInfo() : is_constant(true), needs_slicing(false), child_list_info(list_entry_t(0, 0)) {
	}
	bool is_constant;
	bool needs_slicing;
	list_entry_t child_list_info;
};

//! Vector of values of a specified PhysicalType.
class Vector {
	friend struct ConstantVector;
	friend struct DictionaryVector;
	friend struct FlatVector;
	friend struct ListVector;
	friend struct StringVector;
	friend struct FSSTVector;
	friend struct StructVector;
	friend struct UnionVector;
	friend struct SequenceVector;
	friend struct ArrayVector;

	friend class DataChunk;
	friend class VectorCacheBuffer;

public:
	//! Create a vector that references the other vector
	GOOSE_API Vector(Vector &other);
	//! Create a vector that slices another vector
	GOOSE_API explicit Vector(const Vector &other, const SelectionVector &sel, idx_t count);
	//! Create a vector that slices another vector between a pair of offsets
	GOOSE_API explicit Vector(const Vector &other, idx_t offset, idx_t end);
	//! Create a vector of size one holding the passed on value
	GOOSE_API explicit Vector(const Value &value);
	//! Create a vector of size tuple_count (non-standard)
	GOOSE_API explicit Vector(LogicalType type, idx_t capacity = STANDARD_VECTOR_SIZE);
	//! Create an empty standard vector with a type, equivalent to calling Vector(type, true, false)
	GOOSE_API explicit Vector(const VectorCache &cache);
	//! Create a non-owning vector that references the specified data
	GOOSE_API Vector(LogicalType type, data_ptr_t dataptr);
	//! Create an owning vector that holds at most STANDARD_VECTOR_SIZE entries.
	/*!
	    Create a new vector
	    If create_data is true, the vector will be an owning empty vector.
	    If initialize_to_zero is true, the allocated data will be zero-initialized.
	*/
	GOOSE_API Vector(LogicalType type, bool create_data, bool initialize_to_zero,
	                  idx_t capacity = STANDARD_VECTOR_SIZE);
	// implicit copying of Vectors is not allowed
	Vector(const Vector &) = delete;
	// but moving of vectors is allowed
	GOOSE_API Vector(Vector &&other) noexcept;

public:
	//! Create a vector that references the specified value.
	GOOSE_API void Reference(const Value &value);
	//! Causes this vector to reference the data held by the other vector.
	//! The type of the "other" vector should match the type of this vector
	GOOSE_API void Reference(const Vector &other);
	//! Reinterpret the data of the other vector as the type of this vector
	//! Note that this takes the data of the other vector as-is and places it in this vector
	//! Without changing the type of this vector
	GOOSE_API void Reinterpret(const Vector &other);

	//! Causes this vector to reference the data held by the other vector, changes the type if required.
	GOOSE_API void ReferenceAndSetType(const Vector &other);

	//! Resets a vector from a vector cache.
	//! This turns the vector back into an empty FlatVector with STANDARD_VECTOR_SIZE entries.
	//! The VectorCache is used so this can be done without requiring any allocations.
	GOOSE_API void ResetFromCache(const VectorCache &cache);

	//! Creates a reference to a slice of the other vector
	GOOSE_API void Slice(const Vector &other, idx_t offset, idx_t end);
	//! Creates a reference to a slice of the other vector
	GOOSE_API void Slice(const Vector &other, const SelectionVector &sel, idx_t count);
	//! Turns the vector into a dictionary vector with the specified dictionary
	GOOSE_API void Slice(const SelectionVector &sel, idx_t count);
	//! Slice the vector, keeping the result around in a cache or potentially using the cache instead of slicing
	GOOSE_API void Slice(const SelectionVector &sel, idx_t count, SelCache &cache);
	//! Turn this vector into a dictionary vector
	GOOSE_API void Dictionary(idx_t dictionary_size, const SelectionVector &sel, idx_t count);
	//! Creates a reference to a dictionary of the other vector
	GOOSE_API void Dictionary(Vector &dict, idx_t dictionary_size, const SelectionVector &sel, idx_t count);
	//! Creates a dictionary on the reusable dict
	GOOSE_API void Dictionary(buffer_ptr<VectorChildBuffer> reusable_dict, const SelectionVector &sel);

	//! Creates the data of this vector with the specified type. Any data that
	//! is currently in the vector is destroyed.
	GOOSE_API void Initialize(bool initialize_to_zero = false, idx_t capacity = STANDARD_VECTOR_SIZE);

	//! Converts this Vector to a printable string representation
	GOOSE_API string ToString(idx_t count) const;
	GOOSE_API void Print(idx_t count) const;

	GOOSE_API string ToString() const;
	GOOSE_API void Print() const;

	//! Flatten the vector, removing any compression and turning it into a FLAT_VECTOR
	GOOSE_API void Flatten(idx_t count);
	GOOSE_API void Flatten(const SelectionVector &sel, idx_t count);
	//! Creates a UnifiedVectorFormat of a vector
	//! The UnifiedVectorFormat allows efficient reading of vectors regardless of their vector type
	//! It contains (1) a data pointer, (2) a validity mask, and (3) a selection vector
	//! Access to the individual vector elements can be performed through data_pointer[sel_idx[i]]/validity[sel_idx[i]]
	//! The most common vector types (flat, constant & dictionary) can be converted to the canonical format "for free"
	//! ToUnifiedFormat was originally called Orrify, as a tribute to Orri Erling who came up with it
	GOOSE_API void ToUnifiedFormat(idx_t count, UnifiedVectorFormat &data);
	//! Recursively calls UnifiedVectorFormat on a vector and its child vectors (for nested types)
	static void RecursiveToUnifiedFormat(Vector &input, idx_t count, RecursiveUnifiedVectorFormat &data);

	//! Turn the vector into a sequence vector
	GOOSE_API void Sequence(int64_t start, int64_t increment, idx_t count);

	//! Verify that the Vector is in a consistent, not corrupt state. DEBUG
	//! FUNCTION ONLY!
	GOOSE_API void Verify(idx_t count);
	//! Asserts that the CheckMapValidity returns MapInvalidReason::VALID
	GOOSE_API static void VerifyMap(Vector &map, const SelectionVector &sel, idx_t count);
	GOOSE_API static void VerifyUnion(Vector &map, const SelectionVector &sel, idx_t count);
	GOOSE_API static void VerifyVariant(Vector &map, const SelectionVector &sel, idx_t count);
	GOOSE_API static void Verify(Vector &vector, const SelectionVector &sel, idx_t count);
	GOOSE_API void UTFVerify(idx_t count);
	GOOSE_API void UTFVerify(const SelectionVector &sel, idx_t count);

	//! Returns the [index] element of the Vector as a Value.
	GOOSE_API Value GetValue(idx_t index) const;
	//! Sets the [index] element of the Vector to the specified Value.
	GOOSE_API void SetValue(idx_t index, const Value &val);

	inline void SetAuxiliary(buffer_ptr<VectorBuffer> new_buffer) {
		auxiliary = std::move(new_buffer);
	};

	inline void CopyBuffer(Vector &other) {
		buffer = other.buffer;
		data = other.data;
	}

	//! Resizes the vector.
	GOOSE_API void Resize(idx_t cur_size, idx_t new_size);
	//! Returns a vector of ResizeInfo containing each (nested) vector to resize.
	GOOSE_API void FindResizeInfos(vector<ResizeInfo> &resize_infos, const idx_t multiplier);

	GOOSE_API void Serialize(Serializer &serializer, idx_t count, bool compressed_serialization = true);
	GOOSE_API void Deserialize(Deserializer &deserializer, idx_t count);

	idx_t GetAllocationSize(idx_t cardinality) const;

	// Getters
	inline VectorType GetVectorType() const {
		return vector_type;
	}
	inline const LogicalType &GetType() const {
		return type;
	}
	inline data_ptr_t GetData() const {
		return data;
	}

	inline buffer_ptr<VectorBuffer> GetAuxiliary() {
		return auxiliary;
	}

	inline buffer_ptr<VectorBuffer> GetBuffer() {
		return buffer;
	}

	// Setters
	GOOSE_API void SetVectorType(VectorType vector_type);

	// Transform vector to an equivalent dictionary vector
	static void DebugTransformToDictionary(Vector &vector, idx_t count);
	// Transform vector to an equivalent nested vector
	static void DebugShuffleNestedVector(Vector &vector, idx_t count);

private:
	//! Returns the [index] element of the Vector as a Value.
	static Value GetValue(const Vector &v, idx_t index);
	//! Returns the [index] element of the Vector as a Value.
	static Value GetValueInternal(const Vector &v, idx_t index);

protected:
	//! The vector type specifies how the data of the vector is physically stored (i.e. if it is a single repeated
	//! constant, if it is compressed)
	VectorType vector_type;
	//! The type of the elements stored in the vector (e.g. integer, float)
	LogicalType type;
	//! A pointer to the data.
	data_ptr_t data;
	//! The validity mask of the vector
	ValidityMask validity;
	//! The main buffer holding the data of the vector
	buffer_ptr<VectorBuffer> buffer;
	//! The buffer holding auxiliary data of the vector
	//! e.g. a string vector uses this to store strings
	buffer_ptr<VectorBuffer> auxiliary;
};

//! The VectorChildBuffer holds a child Vector
class VectorChildBuffer : public VectorBuffer {
public:
	explicit VectorChildBuffer(Vector vector)
	    : VectorBuffer(VectorBufferType::VECTOR_CHILD_BUFFER), data(std::move(vector)),
	      cached_hashes(LogicalType::HASH, nullptr) {
	}

public:
	Vector data;
	//! Optional size/id to uniquely identify re-occurring dictionaries
	optional_idx size;
	string id;
	//! For caching the hashes of a child buffer
	mutex cached_hashes_lock;
	Vector cached_hashes;
};

struct ConstantVector {
	template <class T>
	static void VerifyVectorType(const Vector &vector) {
#ifdef GOOSE_DEBUG_NO_SAFETY
		D_ASSERT(StorageTypeCompatible<T>(vector.GetType().InternalType()));
#else
		if (!StorageTypeCompatible<T>(vector.GetType().InternalType())) {
			throw InternalException("Expected vector of type %s, but found vector of type %s", GetTypeId<T>(),
			                        vector.GetType().InternalType());
		}
#endif
	}

	static inline const_data_ptr_t GetData(const Vector &vector) {
		D_ASSERT(vector.GetVectorType() == VectorType::CONSTANT_VECTOR ||
		         vector.GetVectorType() == VectorType::FLAT_VECTOR);
		return vector.data;
	}
	static inline data_ptr_t GetData(Vector &vector) {
		D_ASSERT(vector.GetVectorType() == VectorType::CONSTANT_VECTOR ||
		         vector.GetVectorType() == VectorType::FLAT_VECTOR);
		return vector.data;
	}
	template <class T>
	static inline const T *GetDataUnsafe(const Vector &vector) {
		return reinterpret_cast<const T *>(GetData(vector));
	}
	template <class T>
	static inline T *GetDataUnsafe(Vector &vector) {
		return reinterpret_cast<T *>(GetData(vector));
	}
	template <class T>
	static inline const T *GetData(const Vector &vector) {
		VerifyVectorType<T>(vector);
		return GetDataUnsafe<T>(vector);
	}
	template <class T>
	static inline T *GetData(Vector &vector) {
		VerifyVectorType<T>(vector);
		return GetDataUnsafe<T>(vector);
	}
	static inline bool IsNull(const Vector &vector) {
		D_ASSERT(vector.GetVectorType() == VectorType::CONSTANT_VECTOR);
		return !vector.validity.RowIsValid(0);
	}
	GOOSE_API static void SetNull(Vector &vector, bool is_null);
	static inline ValidityMask &Validity(Vector &vector) {
		D_ASSERT(vector.GetVectorType() == VectorType::CONSTANT_VECTOR);
		return vector.validity;
	}
	GOOSE_API static const SelectionVector *ZeroSelectionVector(idx_t count, SelectionVector &owned_sel);
	GOOSE_API static const SelectionVector *ZeroSelectionVector();
	//! Turns "vector" into a constant vector by referencing a value within the source vector
	GOOSE_API static void Reference(Vector &vector, Vector &source, idx_t position, idx_t count);

	static const sel_t ZERO_VECTOR[STANDARD_VECTOR_SIZE];
};

struct DictionaryVector {
	static void VerifyDictionary(const Vector &vector) {
#ifdef GOOSE_DEBUG_NO_SAFETY
		D_ASSERT(vector.GetVectorType() == VectorType::DICTIONARY_VECTOR);
#else
		if (vector.GetVectorType() != VectorType::DICTIONARY_VECTOR) {
			throw InternalException(
			    "Operation requires a dictionary vector but a non-dictionary vector was encountered");
		}
#endif
	}
	static inline const SelectionVector &SelVector(const Vector &vector) {
		VerifyDictionary(vector);
		return vector.buffer->Cast<DictionaryBuffer>().GetSelVector();
	}
	static inline SelectionVector &SelVector(Vector &vector) {
		VerifyDictionary(vector);
		return vector.buffer->Cast<DictionaryBuffer>().GetSelVector();
	}
	static inline const Vector &Child(const Vector &vector) {
		VerifyDictionary(vector);
		return vector.auxiliary->Cast<VectorChildBuffer>().data;
	}
	static inline Vector &Child(Vector &vector) {
		VerifyDictionary(vector);
		return vector.auxiliary->Cast<VectorChildBuffer>().data;
	}
	static inline optional_idx DictionarySize(const Vector &vector) {
		VerifyDictionary(vector);
		const auto &child_buffer = vector.auxiliary->Cast<VectorChildBuffer>();
		if (child_buffer.size.IsValid()) {
			return child_buffer.size;
		}
		return vector.buffer->Cast<DictionaryBuffer>().GetDictionarySize();
	}
	static inline const string &DictionaryId(const Vector &vector) {
		VerifyDictionary(vector);
		const auto &child_buffer = vector.auxiliary->Cast<VectorChildBuffer>();
		if (!child_buffer.id.empty()) {
			return child_buffer.id;
		}
		return vector.buffer->Cast<DictionaryBuffer>().GetDictionaryId();
	}
	static inline bool CanCacheHashes(const LogicalType &type) {
		return type.InternalType() == PhysicalType::VARCHAR;
	}
	static inline bool CanCacheHashes(const Vector &vector) {
		return DictionarySize(vector).IsValid() && CanCacheHashes(vector.GetType());
	}
	static buffer_ptr<VectorChildBuffer> CreateReusableDictionary(const LogicalType &type, const idx_t &size);
	static const Vector &GetCachedHashes(Vector &input);
};

struct FlatVector {
	static void VerifyFlatVector(const Vector &vector) {
#ifdef GOOSE_DEBUG_NO_SAFETY
		D_ASSERT(vector.GetVectorType() == VectorType::FLAT_VECTOR);
#else
		if (vector.GetVectorType() != VectorType::FLAT_VECTOR) {
			throw InternalException("Operation requires a flat vector but a non-flat vector was encountered");
		}
#endif
	}

	static inline data_ptr_t GetData(Vector &vector) {
		return ConstantVector::GetData(vector);
	}
	template <class T>
	static inline const T *GetData(const Vector &vector) {
		return ConstantVector::GetData<T>(vector);
	}
	template <class T>
	static inline T *GetData(Vector &vector) {
		return ConstantVector::GetData<T>(vector);
	}
	template <class T>
	static inline const T *GetDataUnsafe(const Vector &vector) {
		return ConstantVector::GetDataUnsafe<T>(vector);
	}
	template <class T>
	static inline T *GetDataUnsafe(Vector &vector) {
		return ConstantVector::GetDataUnsafe<T>(vector);
	}
	static inline void SetData(Vector &vector, data_ptr_t data) {
		D_ASSERT(vector.GetVectorType() == VectorType::FLAT_VECTOR);
		vector.data = data;
	}
	template <class T>
	static inline T GetValue(Vector &vector, idx_t idx) {
		D_ASSERT(vector.GetVectorType() == VectorType::FLAT_VECTOR);
		return FlatVector::GetData<T>(vector)[idx];
	}
	static inline const ValidityMask &Validity(const Vector &vector) {
		VerifyFlatVector(vector);
		return vector.validity;
	}
	static inline ValidityMask &Validity(Vector &vector) {
		VerifyFlatVector(vector);
		return vector.validity;
	}
	static inline void SetValidity(Vector &vector, const ValidityMask &new_validity) {
		VerifyFlatVector(vector);
		vector.validity.Initialize(new_validity);
	}
	GOOSE_API static void SetNull(Vector &vector, idx_t idx, bool is_null);
	static inline bool IsNull(const Vector &vector, idx_t idx) {
		D_ASSERT(vector.GetVectorType() == VectorType::FLAT_VECTOR);
		return !vector.validity.RowIsValid(idx);
	}
	GOOSE_API static const SelectionVector *IncrementalSelectionVector();
};

struct ListVector {
	static inline const list_entry_t *GetData(const Vector &v) {
		if (v.GetVectorType() == VectorType::DICTIONARY_VECTOR) {
			auto &child = DictionaryVector::Child(v);
			return GetData(child);
		}
		return FlatVector::GetData<const list_entry_t>(v);
	}
	static inline list_entry_t *GetData(Vector &v) {
		if (v.GetVectorType() == VectorType::DICTIONARY_VECTOR) {
			auto &child = DictionaryVector::Child(v);
			return GetData(child);
		}
		return FlatVector::GetData<list_entry_t>(v);
	}
	//! Gets a reference to the underlying child-vector of a list
	GOOSE_API static const Vector &GetEntry(const Vector &vector);
	//! Gets a reference to the underlying child-vector of a list
	GOOSE_API static Vector &GetEntry(Vector &vector);
	//! Gets the total size of the underlying child-vector of a list
	GOOSE_API static idx_t GetListSize(const Vector &vector);
	//! Sets the total size of the underlying child-vector of a list
	GOOSE_API static void SetListSize(Vector &vec, idx_t size);
	//! Gets the total capacity of the underlying child-vector of a list
	GOOSE_API static idx_t GetListCapacity(const Vector &vector);
	//! Sets the total capacity of the underlying child-vector of a list
	GOOSE_API static void Reserve(Vector &vec, idx_t required_capacity);
	GOOSE_API static void Append(Vector &target, const Vector &source, idx_t source_size, idx_t source_offset = 0);
	GOOSE_API static void Append(Vector &target, const Vector &source, const SelectionVector &sel, idx_t source_size,
	                              idx_t source_offset = 0);
	GOOSE_API static void PushBack(Vector &target, const Value &insert);
	//! Returns the child_vector of list starting at offset until offset + count, and its length
	GOOSE_API static idx_t GetConsecutiveChildList(Vector &list, Vector &result, idx_t offset, idx_t count);
	//! Returns information to only copy a section of a list child vector
	GOOSE_API static ConsecutiveChildListInfo GetConsecutiveChildListInfo(Vector &list, idx_t offset, idx_t count);
	//! Slice and flatten a child vector to only contain a consecutive subsection of the child entries
	GOOSE_API static void GetConsecutiveChildSelVector(Vector &list, SelectionVector &sel, idx_t offset, idx_t count);
	//! Share the entry of the other list vector
	GOOSE_API static void ReferenceEntry(Vector &vector, Vector &other);

private:
	template <class T>
	static T &GetEntryInternal(T &vector);
};

struct StringVector {
	//! Add a string to the string heap of the vector (auxiliary data)
	GOOSE_API static string_t AddString(Vector &vector, const char *data, idx_t len);
	//! Add a string or a blob to the string heap of the vector (auxiliary data)
	//! This function is the same as ::AddString, except the added data does not need to be valid UTF8
	GOOSE_API static string_t AddStringOrBlob(Vector &vector, const char *data, idx_t len);
	//! Add a string to the string heap of the vector (auxiliary data)
	GOOSE_API static string_t AddString(Vector &vector, const char *data);
	//! Add a string to the string heap of the vector (auxiliary data)
	GOOSE_API static string_t AddString(Vector &vector, string_t data);
	//! Add a string to the string heap of the vector (auxiliary data)
	GOOSE_API static string_t AddString(Vector &vector, const string &data);
	//! Add a string or a blob to the string heap of the vector (auxiliary data)
	//! This function is the same as ::AddString, except the added data does not need to be valid UTF8
	GOOSE_API static string_t AddStringOrBlob(Vector &vector, string_t data);
	//! Allocates an empty string of the specified size, and returns a writable pointer that can be used to store the
	//! result of an operation
	GOOSE_API static string_t EmptyString(Vector &vector, idx_t len);
	//! Returns a reference to the underlying VectorStringBuffer - throws an error if vector is not of type VARCHAR
	GOOSE_API static VectorStringBuffer &GetStringBuffer(Vector &vector);
	//! Adds a reference to a handle that stores strings of this vector
	GOOSE_API static void AddHandle(Vector &vector, BufferHandle handle);
	//! Adds a reference to an unspecified vector buffer that stores strings of this vector
	GOOSE_API static void AddBuffer(Vector &vector, buffer_ptr<VectorBuffer> buffer);
	//! Add a reference from this vector to the string heap of the provided vector
	GOOSE_API static void AddHeapReference(Vector &vector, Vector &other);
};

struct FSSTVector {
	static inline const ValidityMask &Validity(const Vector &vector) {
		D_ASSERT(vector.GetVectorType() == VectorType::FSST_VECTOR);
		return vector.validity;
	}
	static inline ValidityMask &Validity(Vector &vector) {
		D_ASSERT(vector.GetVectorType() == VectorType::FSST_VECTOR);
		return vector.validity;
	}
	static inline void SetValidity(Vector &vector, ValidityMask &new_validity) {
		D_ASSERT(vector.GetVectorType() == VectorType::FSST_VECTOR);
		vector.validity.Initialize(new_validity);
	}
	static inline const_data_ptr_t GetCompressedData(const Vector &vector) {
		D_ASSERT(vector.GetVectorType() == VectorType::FSST_VECTOR);
		return vector.data;
	}
	static inline data_ptr_t GetCompressedData(Vector &vector) {
		D_ASSERT(vector.GetVectorType() == VectorType::FSST_VECTOR);
		return vector.data;
	}
	template <class T>
	static inline const T *GetCompressedData(const Vector &vector) {
		return (const T *)FSSTVector::GetCompressedData(vector);
	}
	template <class T>
	static inline T *GetCompressedData(Vector &vector) {
		return (T *)FSSTVector::GetCompressedData(vector);
	}
	//! Decompresses an FSST_VECTOR into a FLAT_VECTOR. Note: validity is not copied.
	static void DecompressVector(const Vector &src, Vector &dst, idx_t src_offset, idx_t dst_offset, idx_t copy_count,
	                             const SelectionVector *sel);

	GOOSE_API static string_t AddCompressedString(Vector &vector, string_t data);
	GOOSE_API static string_t AddCompressedString(Vector &vector, const char *data, idx_t len);
	GOOSE_API static void RegisterDecoder(Vector &vector, buffer_ptr<void> &goose_fsst_decoder,
	                                       const idx_t string_block_limit);
	GOOSE_API static void *GetDecoder(const Vector &vector);
	GOOSE_API static vector<unsigned char> &GetDecompressBuffer(const Vector &vector);
	//! Setting the string count is required to be able to correctly flatten the vector
	GOOSE_API static void SetCount(Vector &vector, idx_t count);
	GOOSE_API static idx_t GetCount(Vector &vector);
};

enum class MapInvalidReason : uint8_t { VALID, NULL_KEY, DUPLICATE_KEY, NOT_ALIGNED, INVALID_PARAMS };

struct MapVector {
	GOOSE_API static const Vector &GetKeys(const Vector &vector);
	GOOSE_API static const Vector &GetValues(const Vector &vector);
	GOOSE_API static Vector &GetKeys(Vector &vector);
	GOOSE_API static Vector &GetValues(Vector &vector);
	GOOSE_API static MapInvalidReason
	CheckMapValidity(Vector &map, idx_t count, const SelectionVector &sel = *FlatVector::IncrementalSelectionVector());
	GOOSE_API static void EvalMapInvalidReason(MapInvalidReason reason);
	GOOSE_API static void MapConversionVerify(Vector &vector, idx_t count);
};

struct StructVector {
	GOOSE_API static const vector<unique_ptr<Vector>> &GetEntries(const Vector &vector);
	GOOSE_API static vector<unique_ptr<Vector>> &GetEntries(Vector &vector);
};

struct ArrayVector {
	//! Gets a reference to the underlying child-vector of an array
	GOOSE_API static const Vector &GetEntry(const Vector &vector);
	//! Gets a reference to the underlying child-vector of an array
	GOOSE_API static Vector &GetEntry(Vector &vector);
	//! Gets the total size of the underlying child-vector of an array
	GOOSE_API static idx_t GetTotalSize(const Vector &vector);

private:
	template <class T>
	static T &GetEntryInternal(T &vector);
};

struct VariantVector {
	//! Gets a reference to the 'keys' list (dictionary) of a Variant
	GOOSE_API static Vector &GetKeys(Vector &vec);
	GOOSE_API static Vector &GetKeys(const Vector &vec);
	//! Gets a reference to the 'children' list of a Variant
	GOOSE_API static Vector &GetChildren(Vector &vec);
	GOOSE_API static Vector &GetChildren(const Vector &vec);
	//! Gets a reference to the 'keys_index' inside the 'children' list of a Variant
	GOOSE_API static Vector &GetChildrenKeysIndex(Vector &vec);
	GOOSE_API static Vector &GetChildrenKeysIndex(const Vector &vec);
	//! Gets a reference to the 'values_index' inside the 'children' list of a Variant
	GOOSE_API static Vector &GetChildrenValuesIndex(Vector &vec);
	GOOSE_API static Vector &GetChildrenValuesIndex(const Vector &vec);
	//! Gets a reference to the 'values' list of a Variant
	GOOSE_API static Vector &GetValues(Vector &vec);
	GOOSE_API static Vector &GetValues(const Vector &vec);
	//! Gets a reference to the 'type_id' inside the 'values' list of a Variant
	GOOSE_API static Vector &GetValuesTypeId(Vector &vec);
	GOOSE_API static Vector &GetValuesTypeId(const Vector &vec);
	//! Gets a reference to the 'byte_offset' inside the 'values' list of a Variant
	GOOSE_API static Vector &GetValuesByteOffset(Vector &vec);
	GOOSE_API static Vector &GetValuesByteOffset(const Vector &vec);
	//! Gets a reference to the binary blob 'value', which encodes the data of the row
	GOOSE_API static Vector &GetData(Vector &vec);
	GOOSE_API static Vector &GetData(const Vector &vec);
};

enum class UnionInvalidReason : uint8_t {
	VALID,
	TAG_OUT_OF_RANGE,
	NO_MEMBERS,
	VALIDITY_OVERLAP,
	TAG_MISMATCH,
	NULL_TAG
};

struct UnionVector {
	// Unions are stored as structs, but the first child is always the "tag"
	// vector, specifying the currently selected member for that row.
	// The remaining children are the members of the union.
	// INVARIANTS:
	//	1.	Only one member vector (the one "selected" by the tag) can be
	//		non-NULL in each row.
	//
	//	2.	The validity of the tag vector always matches the validity of the
	//		union vector itself.
	//
	//  3.  A valid union cannot have a NULL tag, but the selected member can
	//  	be NULL. therefore, there is a difference between a union that "is"
	//  	NULL and a union that "holds" a NULL. The latter still has a valid
	//  	tag.
	//
	//	4.	For each tag in the tag vector, 0 <= tag < |members|

	//! Get the tag vector of a union vector
	GOOSE_API static const Vector &GetTags(const Vector &v);
	GOOSE_API static Vector &GetTags(Vector &v);

	//! Try to get the tag at the specific flat index of the union vector. Returns false if the tag is NULL.
	//! This will handle and map the index properly for constant and dictionary vectors internally.
	GOOSE_API static bool TryGetTag(const Vector &vector, idx_t index, union_tag_t &tag);

	//! Get the member vector of a union vector by index
	GOOSE_API static const Vector &GetMember(const Vector &vector, idx_t member_index);
	GOOSE_API static Vector &GetMember(Vector &vector, idx_t member_index);

	//! Set every entry in the UnionVector to a specific member.
	//! This is useful to set the entire vector to a single member, e.g. when "creating"
	//! a union to return in a function, when you only have one alternative to return.
	//! if 'keep_tags_for_null' is false, the tags will be set to NULL where the member is NULL.
	//! (the validity of the tag vector will match the selected member vector)
	//! otherwise, they are all set to the 'tag'.
	//! This will also handle invalidation of the non-selected members
	GOOSE_API static void SetToMember(Vector &vector, union_tag_t tag, Vector &member_vector, idx_t count,
	                                   bool keep_tags_for_null);

	GOOSE_API static UnionInvalidReason
	CheckUnionValidity(Vector &vector, idx_t count,
	                   const SelectionVector &sel = *FlatVector::IncrementalSelectionVector());
};

struct SequenceVector {
	static void GetSequence(const Vector &vector, int64_t &start, int64_t &increment, int64_t &sequence_count) {
		D_ASSERT(vector.GetVectorType() == VectorType::SEQUENCE_VECTOR);
		auto data = reinterpret_cast<int64_t *>(vector.buffer->GetData());
		start = data[0];
		increment = data[1];
		sequence_count = data[2];
	}
	static void GetSequence(const Vector &vector, int64_t &start, int64_t &increment) {
		int64_t sequence_count;
		GetSequence(vector, start, increment, sequence_count);
	}
};

} // namespace goose
