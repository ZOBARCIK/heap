#ifndef HEAP_HPP
#define HEAP_HPP

#include <vector>
#include <memory_resource>

template<class T, class Compare = std::less<T> >
using heap_pmr = heap <T,Compare, std::pmr::polymorphic_allocator<T> >;

template <  class T, class Compare = std::less<T>, class Alloc = std::allocator<T> >
class Heap {
public:
    using value_type = T;
    using size_type = std::size_t;
    using allocator_type = Alloc;

    explicit Heap(compare comp = Compare(), allocator_type alloc = Alloc())
        : comp_(comp),data_(alloc)  {}

    template <class It>

    Heap (It first, It last, Compare comp = Compare(), const Alloc &alloc = Alloc())
        : comp_(comp), data_(first, last, alloc) {make_heap_():}

private :
    Compare comp_;
    std::vector<T, Alloc> data_;

    void make_heap_() {
        std::make_heap(data_.begin(), data_.end(), comp_);
    }
};
#endif // HEAP_H