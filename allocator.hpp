#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <memory>    
#include <map>       
#include <iostream>  
#include "heap.hpp"  
#include "heapf.hpp" 
#include "allocator_doc.hpp"
class ByteHeapAllocator {
public:
    explicit ByteHeapAllocator(int total_bytes) //constructor
        : buffer(new unsigned char[total_bytes])
        , buffer_size(total_bytes)
    {
        // init heap storage; capacity = e.g. 1024 blocks
        free_blocks.capacity = 1024;
        free_blocks.size     = 0; // insert via insert_free_block later
        free_blocks.arr      = new FreeBlock[free_blocks.capacity + 1];

        // One big free block: [0, total_bytes)
        FreeBlock initial{0, total_bytes};
        insert_free_block(initial); // put into both map + heap
    }
    //destructor
    ~ByteHeapAllocator() {
        delete[] free_blocks.arr;
        // buffer freed automatically
    }

    void* allocate(int bytes) {
        if (bytes <= 0) return nullptr;
        if (free_blocks.size < 1) return nullptr;

        // Largest free block is at index 1 in the heap
        FreeBlock block = free_blocks.arr[1];

        if (block.size < bytes) {
            // Not enough space anywhere
            return nullptr;
        }

        // Remove this block from BOTH data structures
        remove_free_block(block);

        // Use the first 'bytes' of this block
        int alloc_offset = block.offset;
        int remaining    = block.size - bytes;

        // If leftover space, add it back as a free block
        if (remaining > 0) {
            FreeBlock leftover{alloc_offset + bytes, remaining};
            insert_free_block(leftover);
        }

        return buffer.get() + alloc_offset;
    }

    void deallocate(void* ptr, int bytes) {
        if (!ptr || bytes <= 0) return;

        unsigned char* p = static_cast<unsigned char*>(ptr);
        int offset       = static_cast<int>(p - buffer.get());

        FreeBlock fb{offset, bytes};

        // ---- Coalescing with neighbors ----

        // Find the first block with offset >= fb.offset
        auto it = free_by_offset.lower_bound(fb.offset);

        // Check previous neighbor (block with the largest offset < fb.offset)
        if (it != free_by_offset.begin()) {
            auto prev = std::prev(it);
            const FreeBlock& pb = prev->second;

            // if previous block ends where fb starts: merge
            if (pb.offset + pb.size == fb.offset) {
                fb.offset = pb.offset;
                fb.size  += pb.size;

                remove_free_block(pb); // remove old block from map + heap
            }
        }

        // after maybe change to fb.offset, recompute 'it'
        it = free_by_offset.lower_bound(fb.offset);

        // check next neighbor (block with the smallest offset >= fb.offset)
        if (it != free_by_offset.end()) {
            const FreeBlock& nb = it->second;

            // if fb ends where next block starts: merge
            if (fb.offset + fb.size == nb.offset) {
                fb.size += nb.size;

                remove_free_block(nb); // remove neighbor from map + heap
            }
        }

        // Insert the merged block back into both structures
        insert_free_block(fb);
    }

    //for debugging
    void print_free_blocks() const {
        std::cout << "Free blocks (heap size = " << free_blocks.size << "):\n";
        for (int i = 1; i <= free_blocks.size; ++i) {
            std::cout << "  [" << i << "] offset=" << free_blocks.arr[i].offset
                      << " size="   << free_blocks.arr[i].size << "\n";
        }
        std::cout << std::endl;
    }

private:
    std::unique_ptr<unsigned char[]> buffer;
    int buffer_size;

    block_heap free_blocks;
    std::map<int, FreeBlock> free_by_offset; // key = offset

    // insert a free block into the map and the heap
    void insert_free_block(const FreeBlock& fb) {
        // add to map (by offset)
        free_by_offset[fb.offset] = fb;

        // add to max heap (by size)
        heap_insert(free_blocks, fb);
    }

    // remove a free block from the map and the heap
    void remove_free_block(const FreeBlock& fb) {
        // Remove from map
        auto it = free_by_offset.find(fb.offset);
        if (it != free_by_offset.end()) {
            free_by_offset.erase(it);
        }

        // remove from heap, find the block with same offset & size
        int idx = -1;
        for (int i = 1; i <= free_blocks.size; ++i) {
            if (free_blocks.arr[i].offset == fb.offset &&
                free_blocks.arr[i].size   == fb.size) {
                idx = i;
                break;
            }
        }

        // move last element into this position and shrink
        free_blocks.arr[idx] = free_blocks.arr[free_blocks.size];
        free_blocks.size--;

        // rebuild heap to keep it valid 
        if (free_blocks.size >= 1) {
            build_max_heap(free_blocks);
        }
    }
};

#endif // ALLOCATOR_HPP
