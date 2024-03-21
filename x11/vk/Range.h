#ifndef RANGE_H
#define RANGE_H

#include <algorithm>
#include <stdint.h>
#include <vector>

namespace BR {

struct Range {
    size_t start;
    size_t size;
};

class Ranges {
  public:
    std::vector<Range> ranges;
    void add(const Range &next);
};

} // namespace BR

#endif // RANGE_H
