
#include "Range.h"

namespace BR {

static bool isIn(size_t val, size_t lowerInclusive, size_t size) {
    return val >= lowerInclusive && val < lowerInclusive + size;
}

void Ranges::add(const Range &next) {
    for (Range &exist : ranges) {

        if (isIn(exist.start, next.start, next.size) ||
            isIn(exist.start + exist.size, next.start, next.size) ||
            isIn(next.start, exist.start, exist.size) ||
            isIn(next.start + exist.size, exist.start, exist.size)) {
            size_t min = std::min(exist.start, next.start);
            size_t max =
                std::max(next.start + next.size, exist.start + exist.size);

            exist.start = min;
            exist.size = max - min;
            return;
        }
    }

    ranges.push_back(next);
}

} // namespace BR
