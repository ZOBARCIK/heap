#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <memory>    // std::unique_ptr
#include <iostream>  // std::cout, std::endl
#include "heap.hpp"  // FreeBlock, block_heap
#include "heapf.hpp" // max_heapify, heap_insert for block_heap

class ByteHeapAllocator {
public:
    explicit ByteHeapAllocator(int total_bytes)
        : buffer(new unsigned char[total_bytes])
        , buffer_size(total_bytes)
    {
        free_blocks.capacity = 1024;
        free_blocks.size     = 1; // first valid index is 1
        free_blocks.arr      = new FreeBlock[free_blocks.capacity + 1];

        // one big free block
        free_blocks.arr[1] = {0, total_bytes};
    }

    ~ByteHeapAllocator() {
        delete[] free_blocks.arr;
    }

    void* allocate(int bytes) {
        if (bytes <= 0) return nullptr;
        if (free_blocks.size < 1) return nullptr;

        FreeBlock block = free_blocks.arr[1];

        if (block.size < bytes) {
            return nullptr;
        }

        free_blocks.arr[1] = free_blocks.arr[free_blocks.size];
        free_blocks.size--;
        if (free_blocks.size >= 1)
            max_heapify(free_blocks, 1);

        int alloc_offset = block.offset;
        int remaining    = block.size - bytes;

        if (remaining > 0) {
            FreeBlock leftover{alloc_offset + bytes, remaining};
            heap_insert(free_blocks, leftover);
        }

        return buffer.get() + alloc_offset;
    }

    void deallocate(void* ptr, int bytes) {
        if (!ptr || bytes <= 0) return;

        unsigned char* p = static_cast<unsigned char*>(ptr);
        int offset       = static_cast<int>(p - buffer.get());

        FreeBlock fb{offset, bytes};
        heap_insert(free_blocks, fb);
    }

   
    void print_free_blocks() const {
        std::cout << "Free blocks (heap size = " << free_blocks.size << "):\n";
        for (int i = 1; i <= free_blocks.size; ++i) {
            std::cout << "  [" << i << "] offset=" << free_blocks.arr[i].offset
                      << " size=" << free_blocks.arr[i].size << '\n';
        }
        std::cout << std::endl;
    }

private:
    std::unique_ptr<unsigned char[]> buffer;
    int buffer_size;

    block_heap free_blocks;
};

#endif // ALLOCATOR_HPP
