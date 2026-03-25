#include <goose/storage/statistics/segment_statistics.h>

#include <goose/common/exception.h>

namespace goose {

SegmentStatistics::SegmentStatistics(LogicalType type) : statistics(BaseStatistics::CreateEmpty(std::move(type))) {
}

SegmentStatistics::SegmentStatistics(BaseStatistics stats) : statistics(std::move(stats)) {
}

} // namespace goose
