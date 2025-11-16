#ifndef HEAP_HPP
#define HEAP_HPP

struct heap {
    int* arr   = nullptr;
    int  size  = 0;
    int  capacity = 0;
};

// free memory block: [offset, offset + size)
struct FreeBlock {
    int offset; // starting index into the buffer
    int size;   // number of bytes
};

// heap of FreeBlock, 1-based array arr[1..size]
struct block_heap {
    FreeBlock* arr     = nullptr;
    int        size    = 0;
    int        capacity = 0;
};

#endif // HEAP_HPP
