#ifndef HEAP_MEMORY_RESOURCE_DOC_HPP
#define HEAP_MEMORY_RESOURCE_DOC_HPP

#include "heap_memory_resource.hpp"

/// @brief PMR wrapper around ByteHeapAllocator.
/// @details
/// HeapMemoryResource adapts a ByteHeapAllocator to the std::pmr::memory_resource
/// interface, so that standard PMR-aware containers (std::pmr::vector, std::pmr::string,
/// etc.) can allocate their storage from the custom heap-based arena.
///
/// Typical usage:
/// @code
///   ByteHeapAllocator arena(1024);                // 1 KB arena
///   HeapMemoryResource resource(arena);           // PMR adapter
///   std::pmr::vector<int> v(&resource);           // PMR vector using the arena
///   v.push_back(42);
/// @endcode
class HeapMemoryResource : public std::pmr::memory_resource {
public:
    /// @brief Constructs a PMR memory resource that forwards to a ByteHeapAllocator.
    /// @details The resource does not own @p alloc; the caller is responsible for
    ///          ensuring the allocator outlives this memory resource.
/// @param alloc Reference to the underlying ByteHeapAllocator that manages memory.
    explicit HeapMemoryResource(ByteHeapAllocator& alloc);

protected:
    /// @brief Allocates memory via the underlying ByteHeapAllocator.
/// @details Called by PMR-aware containers when they need dynamic storage.
///          Currently, the alignment parameter is ignored and allocation is done
///          purely in terms of the requested @p bytes.
/// @param bytes Number of bytes to allocate.
/// @param alignment Alignment requested by the caller (currently ignored).
/// @return Pointer to allocated storage, or nullptr on failure.
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;

    /// @brief Deallocates memory previously allocated by this resource.
/// @details Forwards to ByteHeapAllocator::deallocate, ignoring the alignment.
/// @param p Pointer to the memory to deallocate.
/// @param bytes Size of the memory block that was originally allocated.
/// @param alignment Alignment that was requested on allocation (currently ignored).
    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;

    /// @brief Compares this memory resource to another for equality.
/// @details For this simple implementation, two resources are considered equal only
///          if they are the same object (i.e. have the same address).
/// @param other Another memory_resource to compare with.
/// @return @c true if and only if @p other is the same object as this one.
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

private:
    // Underlying heap-based allocator used for all allocations.
    ByteHeapAllocator& alloc_;
};

#endif // HEAP_MEMORY_RESOURCE_DOC_HPP
