#ifndef HEAP_MEMORY_RESOURCE_HPP
#define HEAP_MEMORY_RESOURCE_HPP

#include <memory_resource> // std::pmr::memory_resource
#include "allocator.hpp"   // ByteHeapAllocator
#include "heap_memory_resource_doc.hpp"
class HeapMemoryResource : public std::pmr::memory_resource {
public:
    // store a reference to the arena
    explicit HeapMemoryResource(ByteHeapAllocator& alloc)
        : alloc_(alloc) {}

protected:
    // PMR calls this when a container wants to allocate
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        // alignment is ignored for now 
        (void)alignment; // suppress unused warning for now

        return alloc_.allocate(static_cast<int>(bytes));
    }

    // PMR calls this when a container frees memory
    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
        (void)alignment; // ignore for now

        alloc_.deallocate(p, static_cast<int>(bytes));
    }

    // compare two resources for equality
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        // simplest: only equal if it's literally the same object
        return this == &other;
    }

private:
    ByteHeapAllocator& alloc_;
};

#endif // HEAP_MEMORY_RESOURCE_HPP
