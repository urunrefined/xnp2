#ifndef CORE_H_
#define CORE_H_

#include <stddef.h>
#include <stdint.h>

namespace BR {

template <typename T1, size_t N> constexpr size_t arraySize(T1 (&)[N]) {
    return N;
}

} // namespace BR

#endif // CORE
