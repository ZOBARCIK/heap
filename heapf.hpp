#ifndef HEAPF_HPP
#define HEAPF_HPP

#include "heap.hpp"
#include <algorithm>
#include <cstdio>

// ---------- int min-heap on `heap` ----------

// heap array is 1-based: arr[1..size]
inline void min_heapify(heap &h, int index)
{
    int smallest = index;
    int left  = 2 * index;
    int right = 2 * index + 1;

    if (left <= h.size && h.arr[left] < h.arr[smallest])
        smallest = left;

    if (right <= h.size && h.arr[right] < h.arr[smallest])
        smallest = right;

    if (smallest != index)
    {
        std::swap(h.arr[index], h.arr[smallest]);
        min_heapify(h, smallest);
    }
}

inline void build_min_heap(heap &h)
{
    for (int i = h.size / 2; i >= 1; --i)
    {
        min_heapify(h, i);
    }
}

// Removes the minimum element (root) and restores min-heap property
inline void heap_remove_min(heap &h)
{
    if (h.size == 0)
        return; // Heap is empty

    // Move the last element to the root
    h.arr[1] = h.arr[h.size];
    h.size--;

    // Restore the min-heap property
    min_heapify(h, 1);
}


// ---------- FreeBlock max-heap on `block_heap` ----------

// Keep the max-heap property by FreeBlock::size
inline void max_heapify(block_heap &h, int index)
{
    int largest = index;
    int left  = 2 * index;
    int right = 2 * index + 1;

    if (left <= h.size && h.arr[left].size > h.arr[largest].size)
        largest = left;

    if (right <= h.size && h.arr[right].size > h.arr[largest].size)
        largest = right;

    if (largest != index)
    {
        std::swap(h.arr[index], h.arr[largest]);
        max_heapify(h, largest);
    }
}

inline void build_max_heap(block_heap &h)
{
    for (int i = h.size / 2; i >= 1; --i)
    {
        max_heapify(h, i);
    }
}

// Insert a free block into the max-heap (by size)
inline void heap_insert(block_heap &h, FreeBlock fb)
{
    if (h.size == h.capacity)
    {
        // TODO: grow the array or handle error
        return;
    }

    h.size++;
    int index = h.size;
    h.arr[index] = fb;

    while (index > 1)
    {
        int parent = index / 2;
        if (h.arr[parent].size < h.arr[index].size)
        {
            std::swap(h.arr[parent], h.arr[index]);
            index = parent;
        }
        else
        {
            return;
        }
    }
}




#endif // HEAPF_HPP
