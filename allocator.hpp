#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <memory>    // std::unique_ptr
#include <map>       // std::map
#include <iostream>  // optional, for debug
#include "heap.hpp"  // FreeBlock, block_heap
#include "heapf.hpp" // max_heapify, heap_insert for block_heap
#include "allocator_doc.hpp"
class ByteHeapAllocator {
public:
    explicit ByteHeapAllocator(int total_bytes)
        : buffer(new unsigned char[total_bytes])
        , buffer_size(total_bytes)
    {
        // Init heap storage; capacity = e.g. 1024 blocks
        free_blocks.capacity = 1024;
        free_blocks.size     = 0; // we'll insert via insert_free_block
        free_blocks.arr      = new FreeBlock[free_blocks.capacity + 1];

        // One big free block: [0, total_bytes)
        FreeBlock initial{0, total_bytes};
        insert_free_block(initial); // put into both map + heap
    }

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

            // If previous block ends exactly where fb starts: merge
            if (pb.offset + pb.size == fb.offset) {
                fb.offset = pb.offset;
                fb.size  += pb.size;

                remove_free_block(pb); // remove old block from map + heap
            }
        }

        // After possible change to fb.offset, recompute 'it'
        it = free_by_offset.lower_bound(fb.offset);

        // Check next neighbor (block with the smallest offset >= fb.offset)
        if (it != free_by_offset.end()) {
            const FreeBlock& nb = it->second;

            // If fb ends exactly where next block starts: merge
            if (fb.offset + fb.size == nb.offset) {
                fb.size += nb.size;

                remove_free_block(nb); // remove neighbor from map + heap
            }
        }

        // Insert the merged block back into both structures
        insert_free_block(fb);
    }

    // Optional: for debugging
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

    // Insert a free block into BOTH the map and the heap
    void insert_free_block(const FreeBlock& fb) {
        // Add to map (ordered by offset)
        free_by_offset[fb.offset] = fb;

        // Add to heap (max-heap by size)
        heap_insert(free_blocks, fb);
    }

    // Remove a free block from BOTH the map and the heap
    void remove_free_block(const FreeBlock& fb) {
        // Remove from map
        auto it = free_by_offset.find(fb.offset);
        if (it != free_by_offset.end()) {
            free_by_offset.erase(it);
        }

        // Remove from heap: find the block with same offset & size
        int idx = -1;
        for (int i = 1; i <= free_blocks.size; ++i) {
            if (free_blocks.arr[i].offset == fb.offset &&
                free_blocks.arr[i].size   == fb.size) {
                idx = i;
                break;
            }
        }
        if (idx == -1) {
            // Should not happen if things are consistent, but be safe.
            return;
        }

        // Move last element into this position and shrink
        free_blocks.arr[idx] = free_blocks.arr[free_blocks.size];
        free_blocks.size--;

        // Rebuild heap to keep it valid (simple, O(n), fine for now)
        if (free_blocks.size >= 1) {
            build_max_heap(free_blocks);
        }
    }
};

#endif // ALLOCATOR_HPP
