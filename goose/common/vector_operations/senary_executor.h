// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <goose/common/types/data_chunk.h>

#include <functional>

namespace goose {

struct SenaryExecutor {
	static const size_t NCOLS = 6;

	template <class TA, class TB, class TC, class TD, class TE, class TF, class TR,
	          class FUN = std::function<TR(TA, TB, TC, TD, TE, TF)>>
	static void Execute(DataChunk &input, Vector &result, FUN fun) {
		D_ASSERT(input.ColumnCount() >= NCOLS);
		const auto count = input.size();

		bool all_constant = true;
		bool any_null = false;
		for (const auto &v : input.data) {
			if (v.GetVectorType() == VectorType::CONSTANT_VECTOR) {
				if (ConstantVector::IsNull(v)) {
					any_null = true;
				}
			} else {
				all_constant = false;
				break;
			}
		}

		if (all_constant) {
			result.SetVectorType(VectorType::CONSTANT_VECTOR);
			if (any_null) {
				ConstantVector::SetNull(result, true);
			} else {
				auto adata = ConstantVector::GetData<TA>(input.data[0]);
				auto bdata = ConstantVector::GetData<TB>(input.data[1]);
				auto cdata = ConstantVector::GetData<TC>(input.data[2]);
				auto ddata = ConstantVector::GetData<TD>(input.data[3]);
				auto edata = ConstantVector::GetData<TE>(input.data[4]);
				auto fdata = ConstantVector::GetData<TF>(input.data[5]);
				auto result_data = ConstantVector::GetData<TR>(result);
				result_data[0] = fun(*adata, *bdata, *cdata, *ddata, *edata, *fdata);
			}
		} else {
			result.SetVectorType(VectorType::FLAT_VECTOR);
			auto result_data = FlatVector::GetData<TR>(result);
			auto &result_validity = FlatVector::Validity(result);

			bool all_valid = true;
			vector<UnifiedVectorFormat> vdata(NCOLS);
			for (size_t c = 0; c < NCOLS; ++c) {
				input.data[c].ToUnifiedFormat(count, vdata[c]);
				all_valid = all_valid && vdata[c].validity.AllValid();
			}

			auto adata = (const TA *)(vdata[0].data);
			auto bdata = (const TB *)(vdata[1].data);
			auto cdata = (const TC *)(vdata[2].data);
			auto ddata = (const TD *)(vdata[3].data);
			auto edata = (const TE *)(vdata[4].data);
			auto fdata = (const TF *)(vdata[5].data);

			vector<idx_t> idx(NCOLS);
			if (all_valid) {
				for (idx_t r = 0; r < count; ++r) {
					for (size_t c = 0; c < NCOLS; ++c) {
						idx[c] = vdata[c].sel->get_index(r);
					}
					result_data[r] =
					    fun(adata[idx[0]], bdata[idx[1]], cdata[idx[2]], ddata[idx[3]], edata[idx[4]], fdata[idx[5]]);
				}
			} else {
				for (idx_t r = 0; r < count; ++r) {
					all_valid = true;
					for (size_t c = 0; c < NCOLS; ++c) {
						idx[c] = vdata[c].sel->get_index(r);
						if (!vdata[c].validity.RowIsValid(idx[c])) {
							result_validity.SetInvalid(r);
							all_valid = false;
							break;
						}
					}
					if (all_valid) {
						result_data[r] = fun(adata[idx[0]], bdata[idx[1]], cdata[idx[2]], ddata[idx[3]], edata[idx[4]],
						                     fdata[idx[5]]);
					}
				}
			}
		}
	}
};

} // namespace goose
