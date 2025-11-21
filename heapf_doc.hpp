#ifndef HEAP_DOC_HPP
#define HEAP_DOC_HPP

#include "heapf.hpp"

/// @brief Restores the min-heap property for the subtree rooted at a given index.
/// @details Assumes the left and right subtrees of @p index are already valid min-heaps.
///          If the element at @p index is larger than any of its children, it swaps with
///          the smallest child and recursively fixes that subtree.
/// @param h Reference to the integer heap structure (1-based array).
/// @param index Index of the subtree root to heapify (1-based).
/// @usage min_heapify(h, index);
void min_heapify(heap &h, int index);


/// @brief Builds a min-heap from an unordered array in a heap.
/// @details Starts from the last non-leaf node and calls min_heapify on each node
///          going upward to the root. After this call, @p h satisfies the min-heap property.
/// @param h Reference to the integer heap structure (1-based array).
/// @usage build_min_heap(h);
void build_min_heap(heap &h);


/// @brief Removes the minimum element (root) from a min-heap.
/// @details Replaces the root element with the last element in the heap, decreases the size,
///          and calls min_heapify on the root to restore the min-heap property.
///          Does nothing if the heap is empty.
/// @param h Reference to the integer heap structure.
/// @usage heap_remove_min(h);
void heap_remove_min(heap &h);


/// @brief Restores the max-heap property for a FreeBlock heap subtree.
/// @details Used on a @c block_heap where the root element at @p index may violate
///          the max-heap property by size. Compares the block at @p index with its
///          children and swaps with the child having the largest @c size, then
///          recursively heapifies that subtree.
/// @param h Reference to the @c block_heap storing FreeBlock elements.
/// @param index Index of the subtree root to heapify (1-based).
/// @usage max_heapify(free_blocks, index);
void max_heapify(block_heap &h, int index);


/// @brief Builds a max-heap of FreeBlock elements ordered by size.
/// @details Treats @p h.arr[1..h.size] as an unsorted array of FreeBlock values and
///          converts it into a max-heap where the largest block (by @c size) is at the root.
///          Starts from the last non-leaf node and calls max_heapify bottom-up.
/// @param h Reference to the @c block_heap structure.
/// @usage build_max_heap(free_blocks);
void build_max_heap(block_heap &h);


/// @brief Inserts a new free memory block into a FreeBlock max-heap.
/// @details Adds @p fb at the end of the heap array and "bubbles up" the element while
///          its @c size is larger than its parent's size, restoring the max-heap property.
///          If the heap is already at capacity, the function currently does nothing.
/// @param h Reference to the @c block_heap used for free-block management.
/// @param fb The FreeBlock (offset + size) to insert into the heap.
/// @usage heap_insert(free_blocks, block);
void heap_insert(block_heap &h, FreeBlock fb);

#endif // HEAP_DOC_HPP
