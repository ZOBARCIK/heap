#ifndef ALLOCATOR_DOC_HPP
#define ALLOCATOR_DOC_HPP

#include <cstddef>

/// Documentation-only header for `ByteHeapAllocator`.
///
/// This header provides Doxygen-style documentation and a forward declaration
/// without defining the class, preventing duplicate-definition errors when
/// included alongside the real `allocator.hpp` implementation.
///
/// See `allocator.hpp` for the actual implementation.
class ByteHeapAllocator; // forward declaration for documentation purposes

/**
 * @class ByteHeapAllocator
 * @brief Fixed-size heap-based arena allocator managing a contiguous byte buffer.
 *
 * This allocator owns a single contiguous buffer and manages free regions using
 * a max-heap (ordered by block size) together with a map keyed by offset to
 * support fast coalescing of adjacent free blocks on deallocation.
 *
 * The following member functions are documented here for reference; the real
 * definitions live in `allocator.hpp`.
 */

/**
 * @fn ByteHeapAllocator::ByteHeapAllocator(int total_bytes)
 * @brief Constructs an arena of the given size and initializes the free list.
 * @param total_bytes Total number of bytes managed by the allocator.
 *
 * Allocates an internal buffer of `total_bytes`, prepares the internal heap
 * storage (capacity for free-block entries) and inserts a single initial
 * FreeBlock covering the whole buffer.
 *
 * Complexity: O(1)
 */

/**
 * @fn ByteHeapAllocator::~ByteHeapAllocator()
 * @brief Destructor that releases internal resources.
 *
 * Frees the internal array used for heap bookkeeping; the raw byte buffer is
 * managed by a `std::unique_ptr` and is released automatically.
 *
 * Complexity: O(1)
 */

/**
 * @fn void* ByteHeapAllocator::allocate(int bytes)
 * @brief Allocate a block of at least `bytes` bytes from the arena.
 * @param bytes Number of bytes requested (must be > 0).
 * @return Pointer to allocated memory within the arena, or `nullptr` on failure.
 *
 * Selection policy: picks the largest free block (root of the max-heap), and
 * if it is large enough, removes it from the free lists, splits off the
 * requested portion and reinserts any leftover as a new free block.
 *
 * Alignment: no explicit alignment guarantees are provided (byte-granular).
 *
 * Complexity: O(n) worst-case due to heap rebuild on removal (current
 * implementation removes by linear search then rebuilds the heap); typically
 * amortized better for small heaps.
 */

/**
 * @fn void ByteHeapAllocator::deallocate(void* ptr, int bytes)
 * @brief Return a previously allocated block back to the free lists.
 * @param ptr Pointer previously returned by `allocate()`.
 * @param bytes Size in bytes of the block being freed.
 *
 * The function computes the offset of `ptr` in the arena buffer, constructs a
 * FreeBlock for that region, attempts to coalesce with immediate neighbors
 * (previous and next blocks found via the offset-ordered `std::map`), removes
 * any merged neighbors from the free lists and reinserts the final merged
 * block.
 *
 * Safety: noop if `ptr` is `nullptr` or `bytes <= 0`.
 *
 * Complexity: O(log m) map lookups plus O(n) heap rebuild when neighbors are
 * removed (where m is number of free blocks and n is heap size in rebuild).
 */

/**
 * @fn void ByteHeapAllocator::print_free_blocks() const
 * @brief Debug helper that prints the contents of the heap of free blocks.
 *
 * Iterates the internal `block_heap` array (1-based heap) and prints each
 * `FreeBlock`'s index, offset and size to `std::cout`.
 */

/**
 * @fn void ByteHeapAllocator::insert_free_block(const FreeBlock& fb)
 * @brief Inserts a `FreeBlock` into both the offset-ordered map and the heap.
 * @param fb The free block to insert.
 *
 * Called internally when creating or returning free regions; ensures the block
 * is present in both bookkeeping structures.
 */

/**
 * @fn void ByteHeapAllocator::remove_free_block(const FreeBlock& fb)
 * @brief Removes a `FreeBlock` from both the map and the heap.
 * @param fb The free block to remove (matched by offset and size).
 *
 * Implementation note: the current removal searches the heap linearly to find
 * the matching entry, swaps the last element into the hole and decrements the
 * heap size, then rebuilds the heap to restore the max-heap property.
 */

#endif // ALLOCATOR_DOC_HPP
///
