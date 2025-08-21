#ifndef HEAP_DOC_HPP
#define HEAP_DOC_HPP
#include "heapf.hpp"

/// @brief Restores the min-heap property for the subtree rooted at index.
/// @details If the parent is greater than its children, swaps with the smallest child and recursively heapifies.
/// @param h Reference to the heap structure.
/// @param index Index of the subtree root.
/// @usage min_heapify(h, index);
void min_heapify(heap &h, int index);


/// @brief Builds a min-heap from an unordered heap.
/// @details Calls min_heapify on all non-leaf nodes from bottom up.
/// @param h Reference to the heap structure.
/// @usage build_min_heap(h);
void build_min_heap(heap &h);


/// @brief Restores the max-heap property for the subtree rooted at index.
/// @details If the parent is smaller than its children, swaps with the largest child and recursively heapifies.
/// @param h Reference to the heap structure.
/// @param index Index of the subtree root.
/// @usage max_heapify(h, index);
void max_heapify(heap &h, int index);


/// @brief Builds a max-heap from an unordered heap.
/// @details Calls max_heapify on all non-leaf nodes from bottom up.
/// @param h Reference to the heap structure.
/// @usage build_max_heap(h);
void build_max_heap(heap &h);


/// @brief Inserts a value into the min-heap.
/// @details Adds the value at the end and "bubbles up" to restore the min-heap property.
/// @param h Reference to the heap structure.
/// @param val Value to insert.
/// @usage heap_insert(h, value);
void heap_insert(heap &h, int val);


/// @brief Removes the minimum element (root) from the min-heap.
/// @details Moves the last element to the root, decreases size, and restores the min-heap property.
/// @param h Reference to the heap structure.
/// @usage heap_remove_min(h);
void heap_remove_min(heap &h);

#endif // HEAP_DOC_H