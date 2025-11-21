#ifndef ALLOCATOR_DOC_HPP
#define ALLOCATOR_DOC_HPP

#include "allocator.hpp"

/// @brief Fixed-size heap-based arena allocator managing a contiguous byte buffer.
/// @details
/// ByteHeapAllocator owns a fixed-size buffer and manages free regions inside it using:
///   - a max-heap of FreeBlock entries (block_heap) ordered by block size, and
///   - a std::map keyed by offset to support neighbor coalescing on deallocation.
/// Allocation requests are satisfied by splitting the largest suitable free block.
///
/// Typical usage:
/// @code
///   ByteHeapAllocator arena(1024);          // 1 KB arena
///   void* p = arena.allocate(128);          // allocate 128 bytes
///   arena.deallocate(p, 128);               // free those 128 bytes
///   arena.print_free_blocks();              // debug the free list
/// @endcode
class ByteHeapAllocator {
public:
    /// @brief Constructs a heap-based arena of the given size in bytes.
    /// @details Allocates an internal buffer of @p total_bytes and initializes the
    ///          free list with a single FreeBlock covering the entire buffer.
    /// @param total_bytes Total size of the managed memory buffer, in bytes.
    explicit ByteHeapAllocator(int total_bytes);

    /// @brief Destructor that releases the internal free-block array and buffer.
    /// @details Frees the internal heap array of FreeBlock entries and lets
    ///          std::unique_ptr automatically release the raw byte buffer.
    ~ByteHeapAllocator();

    /// @brief Allocates a block of memory from the arena.
    /// @details
    ///   - Chooses the largest available FreeBlock from the internal @c block_heap.
    ///   - If the chosen block is large enough, it is removed from the free list,
    ///     split into [used, leftover], and the leftover is reinserted as a new FreeBlock.
    ///   - Returns a pointer into the internal buffer on success, or @c nullptr if there
    ///     is no block large enough to satisfy the request.
    /// @param bytes Number of bytes to allocate (must be > 0).
    /// @return Pointer to a memory region of at least @p bytes, or @c nullptr on failure.
    /// @usage void* p = allocator.allocate(256);
    void* allocate(int bytes);

    /// @brief Deallocates a previously allocated memory block.
    /// @details
    ///   - Converts @p ptr back into an offset inside the internal buffer.
    ///   - Creates a FreeBlock of size @p bytes at that offset.
    ///   - Attempts to coalesce this block with immediate neighbors (previous/next) using
    ///     the map keyed by offset, then reinserts the merged block into both the map
    ///     and the max-heap of free blocks.
    ///   - Does nothing if @p ptr is null or @p bytes is non-positive.
    /// @param ptr Pointer previously returned by allocate() and still owned by the caller.
/// @param bytes Size of the allocated block, in bytes, that is being freed.
/// @usage allocator.deallocate(p, 256);
    void deallocate(void* ptr, int bytes);

    /// @brief Prints the current list of free blocks to std::cout for debugging.
    /// @details Iterates over the internal block_heap and prints each FreeBlock's
    ///          index, offset, and size. Useful for visualizing fragmentation and
    ///          allocator behavior over time.
/// @usage allocator.print_free_blocks();
    void print_free_blocks() const;

private:
    // implementation details are documented in allocator.hpp
};

#endif // ALLOCATOR_DOC_HPP
