#ifndef ARRAYVIEW_H_
#define ARRAYVIEW_H_

#include <assert.h>
#include <unistd.h>

namespace BR {

template <class T1> class ArrayView {

  public:
    const T1 *elements;
    size_t count;

    ArrayView(const T1 *elements_, size_t count_)
        : elements(elements_), count(count_) {}

    template <size_t N>
    ArrayView(const T1 (&elements_)[N]) : elements(&elements_), count(N) {}

    ArrayView(const T1 &elements_, size_t count_)
        : elements(&elements_), count(count_) {}

    ArrayView(const ArrayView<T1> &arr) = default;

    bool empty() const { return count == 0; }

    const T1 *data() const { return elements; }

    size_t size() const { return count; }

    size_t byteSize() const { return count * sizeof(T1); }

    const T1 &operator[](size_t index) {
        assert(index < count);
        return *(elements + index);
    }
};

template <class T1, size_t size> ArrayView<T1> makeArray(T1 (&elements)[size]) {
    return ArrayView<T1>(elements, size);
}

} // Namespace BR

#endif // ARRAYVIEW_H_
