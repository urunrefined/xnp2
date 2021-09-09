#ifndef RANGE_H
#define RANGE_H

#include <vector>
#include <stdint.h>
#include <algorithm>

namespace BR {

struct Range {
	size_t start;
	size_t size;
};

class Ranges {
public:
	std::vector<Range> ranges;
	void add (const Range& next);
};

}

#endif // RANGE_H
