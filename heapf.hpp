#ifndef HEAPF_HPP
#define HEAPF_HPP
#include "heap.hpp"
#include <algorithm>
#include <iostream>


//heap array of index 1
//keep the min-heap property
//if the index(parent) is greater than children, swap it with the smallest child
//then recursively call min_heapify on the swapped child
void min_heapify(heap &h, int index)
{
    int smallest = index;
    int left = 2 * index;
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

//heap array of index 1
//builds a max-heap from the given heap
//it starts from the last non-leaf node and calls mins_heapify on each node
void build_min_heap(heap &h)
{
    for (int i = h.size / 2; i >= 1; i--)
    {
        min_heapify(h, i);
    }

    for (int i = 1; i <= h.size; i++)
    {        printf("%d ", h.arr[i]);
    }
    printf("\n");
}


//heap array of index 1
//keep the max-heap property
//if the index(parent) is smaller than children, swap it with the largest child
//then recursively call max_heapify on the swapped child
void max_heapify(heap &h, int index)
{
    int largest = index;
    int left = 2 * index;
    int right = 2 * index + 1;

    if (left <= h.size && h.arr[left] > h.arr[largest])
        largest = left;

    if (right <= h.size && h.arr[right] > h.arr[largest])
        largest = right;

    if (largest != index)
    {
        std::swap(h.arr[index], h.arr[largest]);
        max_heapify(h, largest);
    }
}

//heap array of index 1
//builds a max-heap from the given heap
//it starts from the last non-leaf node and calls max_heapify on each node
void build_max_heap(heap &h)
{
    for (int i = h.size / 2; i >= 1; i--)
    {
        max_heapify(h, i);
    }

        for (int i = 1; i <= h.size; i++)
    {        printf("%d ", h.arr[i]);
    }
    printf("\n");
}
//heap array of index 1
//increases the size of the heap and inserts the value at the end
//then it checks if the inserted value is smaller than its parent
//if it is, it swaps the value with its parent and continues this process until not 
void heap_insert(heap &h, int val)
{
    h.size++;
    int index = h.size;
    h.arr[index] = val;

    while (index > 1)
    {
        int parent = index / 2;
        if (h.arr[parent] > h.arr[index])
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



//heap array of index 1
//removes the minimum element from the heap
//it moves the last element to the root and decreases the size of the heap
//then it calls min_heapify on the root to restore the min-heap property
void heap_remove_min(heap &h)
{
    if (h.size == 0)
        return; // Heap is empty

    // Move the last element to the root
    h.arr[1] = h.arr[h.size];
    h.size--;

    // Restore the min-heap property
    min_heapify(h, 1);
}









#endif // HEAPF_H


