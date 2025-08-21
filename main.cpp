#include "heap.h"


int main() {
    heap h;

    // Example usage
    h.size = 10; // Assume we have 5 elements
    h.arr[1] = 10;
    h.arr[2] = 20;
    h.arr[3] = 5;
    h.arr[4] = 31;
    h.arr[5] = 69;
    h.arr[6] = 62;
    h.arr[7] = 30;
    h.arr[8] = 25;
    h.arr[9] = 7;
    h.arr[10] = 8;

    //create a min-heap 
    build_min_heap(h); // Call to build the min-heap


    heap_insert(h, 38); // insert value 3 into the min-heap

    //remove the minimum element from the min-heap
    heap_remove_min(h);

    // print
    for (int i = 1; i <= h.size; i++) {
        printf("%d ", h.arr[i]);
    }
    printf("\n"); 

    return 0;
}
