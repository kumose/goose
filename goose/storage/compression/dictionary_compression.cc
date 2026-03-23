#include <goose/storage/compression/dictionary/analyze.h>
#include <goose/storage/compression/dictionary/compression.h>
#include <goose/storage/compression/dictionary/decompression.h>

#include <goose/common/bitpacking.h>
#include <goose/common/numeric_utils.h>
#include <goose/common/types/vector_buffer.h>
#include <goose/function/compression/compression.h>
#include <goose/function/compression_function.h>
#include <goose/storage/segment/uncompressed.h>
#include <goose/storage/string_uncompressed.h>
#include <goose/storage/table/column_data_checkpointer.h>

/*
Data layout per segment:
+------------------------------------------------------+
|                  Header                              |
|   +----------------------------------------------+   |
|   |   dictionary_compression_header_t  header    |   |
|   +----------------------------------------------+   |
|                                                      |
+------------------------------------------------------+
|             Selection Buffer               |
|   +------------------------------------+   |
|   |   uint16_t index_buffer_idx[]      |   |
|   +------------------------------------+   |
|      tuple index -> index buffer idx       |
|                                            |
+--------------------------------------------+
|               Index Buffer                 |
|   +------------------------------------+   |
|   |   uint16_t  dictionary_offset[]    |   |
|   +------------------------------------+   |
|  string_index -> offset in the dictionary  |
|                                            |
+--------------------------------------------+
|                Dictionary                  |
|   +------------------------------------+   |
|   |   uint8_t *raw_string_data         |   |
|   +------------------------------------+   |
|      the string data without lengths       |
|                                            |
+--------------------------------------------+
*/

namespace goose {
    struct DictionaryCompressionStorage {
        static unique_ptr<AnalyzeState> StringInitAnalyze(ColumnData &col_data, PhysicalType type);

        static bool StringAnalyze(AnalyzeState &state_p, Vector &input, idx_t count);

        static idx_t StringFinalAnalyze(AnalyzeState &state_p);

        static unique_ptr<CompressionState> InitCompression(ColumnDataCheckpointData &checkpoint_data,
                                                            unique_ptr<AnalyzeState> state);

        static void Compress(CompressionState &state_p, Vector &scan_vector, idx_t count);

        static void FinalizeCompress(CompressionState &state_p);

        static unique_ptr<SegmentScanState> StringInitScan(const QueryContext &context, ColumnSegment &segment);

        template<bool ALLOW_DICT_VECTORS>
        static void StringScanPartial(ColumnSegment &segment, ColumnScanState &state, idx_t scan_count, Vector &result,
                                      idx_t result_offset);

        static void StringScan(ColumnSegment &segment, ColumnScanState &state, idx_t scan_count, Vector &result);

        static void StringFetchRow(ColumnSegment &segment, ColumnFetchState &state, row_t row_id, Vector &result,
                                   idx_t result_idx);
    };

    //===--------------------------------------------------------------------===//
    // Analyze
    //===--------------------------------------------------------------------===//
    unique_ptr<AnalyzeState> DictionaryCompressionStorage::StringInitAnalyze(ColumnData &col_data, PhysicalType type) {
        auto &storage_manager = col_data.GetStorageManager();
        if (storage_manager.GetStorageVersion() >= 5) {
            // dict_fsst introduced - disable dictionary
            return nullptr;
        }

        CompressionInfo info(col_data.GetBlockManager());
        return make_uniq<DictionaryCompressionAnalyzeState>(info);
    }

    bool DictionaryCompressionStorage::StringAnalyze(AnalyzeState &state_p, Vector &input, idx_t count) {
        auto &state = state_p.Cast<DictionaryCompressionAnalyzeState>();
        return state.analyze_state->UpdateState(input, count);
    }

    idx_t DictionaryCompressionStorage::StringFinalAnalyze(AnalyzeState &state_p) {
        auto &analyze_state = state_p.Cast<DictionaryCompressionAnalyzeState>();
        auto &state = *analyze_state.analyze_state;

        if (state.current_tuple_count != 0) {
            state.UpdateMaxUniqueCount();
        }

        auto width = BitpackingPrimitives::MinimumBitWidth(state.current_unique_count + 1);
        auto req_space = DictionaryCompression::RequiredSpace(state.current_tuple_count, state.current_unique_count,
                                                              state.current_dict_size, width);

        const auto total_space = state.segment_count * state.info.GetBlockSize() + req_space;
        return LossyNumericCast<idx_t>(DictionaryCompression::MINIMUM_COMPRESSION_RATIO * float(total_space));
    }

    //===--------------------------------------------------------------------===//
    // Compress
    //===--------------------------------------------------------------------===//
    unique_ptr<CompressionState> DictionaryCompressionStorage::InitCompression(
        ColumnDataCheckpointData &checkpoint_data,
        unique_ptr<AnalyzeState> state) {
        const auto &analyze_state = state->Cast<DictionaryCompressionAnalyzeState>();
        auto &actual_state = *analyze_state.analyze_state;
        return make_uniq<DictionaryCompressionCompressState>(checkpoint_data, state->info,
                                                             actual_state.max_unique_count_across_segments);
    }

    void DictionaryCompressionStorage::Compress(CompressionState &state_p, Vector &scan_vector, idx_t count) {
        auto &state = state_p.Cast<DictionaryCompressionCompressState>();
        state.UpdateState(scan_vector, count);
    }

    void DictionaryCompressionStorage::FinalizeCompress(CompressionState &state_p) {
        auto &state = state_p.Cast<DictionaryCompressionCompressState>();
        state.Flush(true);
    }

    //===--------------------------------------------------------------------===//
    // Scan
    //===--------------------------------------------------------------------===//
    unique_ptr<SegmentScanState> DictionaryCompressionStorage::StringInitScan(const QueryContext &context,
                                                                              ColumnSegment &segment) {
        auto &buffer_manager = BufferManager::GetBufferManager(segment.db);
        auto state = make_uniq<CompressedStringScanState>(buffer_manager.Pin(segment.block));
        state->Initialize(segment, true);
        return state;
    }

    //===--------------------------------------------------------------------===//
    // Scan base data
    //===--------------------------------------------------------------------===//
    template<bool ALLOW_DICT_VECTORS>
    void DictionaryCompressionStorage::StringScanPartial(ColumnSegment &segment, ColumnScanState &state,
                                                         idx_t scan_count,
                                                         Vector &result, idx_t result_offset) {
        // clear any previously locked buffers and get the primary buffer handle
        auto &scan_state = state.scan_state->Cast<CompressedStringScanState>();

        auto start = state.GetPositionInSegment();
        if (!ALLOW_DICT_VECTORS || scan_count != STANDARD_VECTOR_SIZE) {
            scan_state.ScanToFlatVector(result, result_offset, start, scan_count);
        } else {
            scan_state.ScanToDictionaryVector(segment, result, result_offset, start, scan_count);
        }
    }

    void DictionaryCompressionStorage::StringScan(ColumnSegment &segment, ColumnScanState &state, idx_t scan_count,
                                                  Vector &result) {
        StringScanPartial<true>(segment, state, scan_count, result, 0);
    }

    //===--------------------------------------------------------------------===//
    // Fetch
    //===--------------------------------------------------------------------===//
    void DictionaryCompressionStorage::StringFetchRow(ColumnSegment &segment, ColumnFetchState &state, row_t row_id,
                                                      Vector &result, idx_t result_idx) {
        // fetch a single row from the string segment
        CompressedStringScanState scan_state(state.GetOrInsertHandle(segment));
        scan_state.Initialize(segment, false);
        scan_state.ScanToFlatVector(result, result_idx, NumericCast<idx_t>(row_id), 1);
    }

    //===--------------------------------------------------------------------===//
    // Get Function
    //===--------------------------------------------------------------------===//
    CompressionFunction DictionaryCompressionFun::GetFunction(PhysicalType data_type) {
        return CompressionFunction(
            CompressionType::COMPRESSION_DICTIONARY, data_type, DictionaryCompressionStorage::StringInitAnalyze,
            DictionaryCompressionStorage::StringAnalyze, DictionaryCompressionStorage::StringFinalAnalyze,
            DictionaryCompressionStorage::InitCompression, DictionaryCompressionStorage::Compress,
            DictionaryCompressionStorage::FinalizeCompress, DictionaryCompressionStorage::StringInitScan,
            DictionaryCompressionStorage::StringScan, DictionaryCompressionStorage::StringScanPartial<false>,
            DictionaryCompressionStorage::StringFetchRow, UncompressedFunctions::EmptySkip,
            UncompressedStringStorage::StringInitSegment);
    }

    bool DictionaryCompressionFun::TypeIsSupported(const PhysicalType physical_type) {
        return physical_type == PhysicalType::VARCHAR;
    }
} // namespace goose
