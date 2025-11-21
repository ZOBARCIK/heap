# Bu readme'yi CHATGPTye yazdırdım, sıkılırsanız kb

# Heap-Based Memory Allocator with PMR Integration

This project is a small educational C++ project that builds:

1. A basic **binary heap** data structure (min-heap and max-heap on arrays).
2. A **custom memory allocator** (`ByteHeapAllocator`) that manages a fixed-size byte buffer using a **heap of free blocks** plus coalescing.
3. A **C++17 PMR (`std::pmr`) integration** layer (`HeapMemoryResource`) that lets standard containers like `std::pmr::vector` and `std::pmr::string` allocate memory from your custom allocator instead of the global heap.

The goal is to show the full path:

> heap data structure → free-list/allocator → PMR-compatible memory resource → used by STL containers.

---

## Project Structure

Typical layout:

- `heap.hpp`  
  - Core struct definitions:
    - `heap` – simple heap of `int`.
    - `FreeBlock` – describes a free memory region in the arena.
    - `block_heap` – heap of `FreeBlock` used to manage free memory blocks.

- `heapf.hpp`  
  - Heap algorithm functions:
    - `min_heapify(heap&, int)`
    - `build_min_heap(heap&)`
    - `heap_remove_min(heap&)`
    - `max_heapify(block_heap&, int)`
    - `build_max_heap(block_heap&)`
    - `heap_insert(block_heap&, FreeBlock)`

- `allocator.hpp`  
  - Implements the **byte-level arena allocator**:
    - Class `ByteHeapAllocator`
      - Owns a contiguous byte buffer.
      - Manages free space using:
        - a `block_heap` (max-heap by block size), and
        - a `std::map<int, FreeBlock>` (ordered by offset) for coalescing neighbors.
      - Public API:
        - `void* allocate(int bytes)`
        - `void deallocate(void* ptr, int bytes)`
        - `void print_free_blocks() const`

- `heap_memory_resource.hpp`  
  - Wraps `ByteHeapAllocator` as a `std::pmr::memory_resource`:
    - Class `HeapMemoryResource : public std::pmr::memory_resource`
      - Forwards:
        - `do_allocate` → `ByteHeapAllocator::allocate`
        - `do_deallocate` → `ByteHeapAllocator::deallocate`
      - Lets PMR containers allocate from the heap-based arena.

- `main.cpp`  
  - Example usage:
    - Creates a `ByteHeapAllocator` arena.
    - Wraps it in `HeapMemoryResource`.
    - Allocates and uses `std::pmr::vector<int>` and `std::pmr::string` backed by the custom allocator.
    - Prints free blocks to visualize allocator behavior.

---

## Core Concepts

### 1. Basic Heap Data Structures

The project starts with classic heap operations on a **1-based array** (index 1..size).

- **Min-heap on `heap` (int)**

  ```cpp
  struct heap {
      int* arr = nullptr;
      int  size = 0;
      int  capacity = 0;
  };
  ```

  Functions:

  - `min_heapify(heap& h, int index)`  
    Ensures the subtree rooted at `index` satisfies the min-heap property:
    - `h.arr[index]` is smaller than its children.

  - `build_min_heap(heap& h)`  
    Builds a min-heap from arbitrary values in `h.arr[1..size]`.

  - `heap_remove_min(heap& h)`  
    Removes the root (minimum element), moves the last element to root, and fixes the heap with `min_heapify`.

- **Max-heap on `block_heap` (FreeBlock)**

  ```cpp
  struct FreeBlock {
      int offset; // start index in buffer
      int size;   // length in bytes
  };

  struct block_heap {
      FreeBlock* arr = nullptr; // arr[1..size]
      int size = 0;
      int capacity = 0;
  };
  ```

  Functions:

  - `max_heapify(block_heap& h, int index)`  
    Maintains **max-heap by block size**: each node’s block has size >= its children.

  - `build_max_heap(block_heap& h)`  
    Builds a max-heap of blocks from the current `arr[1..size]`.

  - `heap_insert(block_heap& h, FreeBlock fb)`  
    Inserts a new free block and “bubbles it up” according to its size.

This makes `block_heap` a priority structure for free blocks: the largest free block is always at the root (`arr[1]`).

---

### 2. Byte-Level Heap Allocator (`ByteHeapAllocator`)

`ByteHeapAllocator` implements a simple arena allocator that:

- Allocates a fixed-size byte buffer.
- Tracks free regions using:
  - `block_heap free_blocks`: max-heap ordered by `FreeBlock.size` (for fast allocation of large blocks).
  - `std::map<int, FreeBlock> free_by_offset`: ordered by `offset` (for neighbor coalescing when deallocating).

#### Public API

```cpp
class ByteHeapAllocator {
public:
    explicit ByteHeapAllocator(int total_bytes);
    ~ByteHeapAllocator();

    void* allocate(int bytes);
    void  deallocate(void* ptr, int bytes);

    void print_free_blocks() const;

private:
    std::unique_ptr<unsigned char[]> buffer;
    int buffer_size;

    block_heap free_blocks;
    std::map<int, FreeBlock> free_by_offset;

    void insert_free_block(const FreeBlock& fb);
    void remove_free_block(const FreeBlock& fb);
};
```

#### Initialization

On construction:

- Allocate `buffer` of size `total_bytes`.
- Initialize `free_blocks` and `free_by_offset` with a **single big free block**:

```cpp
FreeBlock initial{0, total_bytes};
insert_free_block(initial);
```

This means the entire arena is initially free.

#### Allocation Logic

```cpp
void* ByteHeapAllocator::allocate(int bytes) {
    if (bytes <= 0) return nullptr;
    if (free_blocks.size < 1) return nullptr;

    // 1. Take largest free block from heap root
    FreeBlock block = free_blocks.arr[1];
    if (block.size < bytes) return nullptr; // not enough space

    // 2. Remove this block from both heap and map
    remove_free_block(block);

    // 3. Use first 'bytes' of this block
    int alloc_offset = block.offset;
    int remaining = block.size - bytes;

    // 4. If there's leftover, reinsert as new free block
    if (remaining > 0) {
        FreeBlock leftover{alloc_offset + bytes, remaining};
        insert_free_block(leftover);
    }

    // 5. Return pointer into our buffer
    return buffer.get() + alloc_offset;
}
```

High-level:

- Always pick the **largest block** to minimize allocation failure.
- Split the selected block into:
  - Used portion.
  - Remaining free portion (if any).

#### Deallocation Logic with Coalescing

On `deallocate(ptr, bytes)`:

1. Convert `ptr` back to an offset:

   ```cpp
   unsigned char* p = static_cast<unsigned char*>(ptr);
   int offset = static_cast<int>(p - buffer.get());
   FreeBlock fb{offset, bytes};
   ```

2. Use `free_by_offset` (a `std::map<int, FreeBlock>`) to find neighbors:

   - Previous block (`prev`) where `prev.offset < fb.offset`.
   - Next block (`next`) where `next.offset >= fb.offset`.

3. Coalesce:

   ```cpp
   // Merge with previous if it ends exactly where fb starts
   if (prev.offset + prev.size == fb.offset) {
       fb.offset = prev.offset;
       fb.size  += prev.size;
       remove_free_block(prev);
   }

   // Recompute iterator, then merge with next if fb ends at next.offset
   if (fb.offset + fb.size == next.offset) {
       fb.size += next.size;
       remove_free_block(next);
   }
   ```

4. Insert the merged `fb` back into both:

   ```cpp
   insert_free_block(fb);
   ```

This ensures adjacent free blocks are merged into larger continuous blocks, reducing fragmentation.

#### Keeping Heap and Map in Sync

- `insert_free_block(fb)`:
  - Inserts `fb` into `free_by_offset[fb.offset]`.
  - Calls `heap_insert(free_blocks, fb)` to push into the heap.

- `remove_free_block(fb)`:
  - Erases `fb` from `free_by_offset` by `fb.offset`.
  - Finds the corresponding `FreeBlock` in `free_blocks.arr[1..size]`.
  - Replaces it with the last element, reduces `size`, then calls `build_max_heap(free_blocks)` to reheapify.

This design trades some performance for simplicity and clarity, which is ideal for learning.

---

### 3. PMR Integration (`HeapMemoryResource`)

To integrate with C++17’s **Polymorphic Memory Resource** (PMR), the project wraps `ByteHeapAllocator` into `std::pmr::memory_resource`:

```cpp
#include <memory_resource>
#include "allocator.hpp"

class HeapMemoryResource : public std::pmr::memory_resource {
public:
    explicit HeapMemoryResource(ByteHeapAllocator& alloc)
        : alloc_(alloc) {}

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        (void)alignment; // alignment currently ignored
        return alloc_.allocate(static_cast<int>(bytes));
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
        (void)alignment; // alignment currently ignored
        alloc_.deallocate(p, static_cast<int>(bytes));
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

private:
    ByteHeapAllocator& alloc_;
};
```

This makes it possible to use **standard PMR containers** like `std::pmr::vector` and `std::pmr::string` with the custom allocator.

---

### Basic PMR Example (`main.cpp`)

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <memory_resource>

#include "allocator.hpp"
#include "heap_memory_resource.hpp"

int main() {
    // 1. Create the custom arena (1 KB buffer)
    ByteHeapAllocator arena(1024);

    // 2. Wrap it as a PMR memory resource
    HeapMemoryResource resource(arena);

    // 3. Create PMR containers backed by the custom allocator
    std::pmr::vector<int> v(&resource);
    std::pmr::basic_string<char> s(&resource); // alias: std::pmr::string

    std::cout << "Initial free blocks:\n";
    arena.print_free_blocks();

    // Use the vector
    for (int i = 0; i < 10; ++i) {
        v.push_back(i * 10);
    }

    // Use the string
    s = "hello pmr";
    s += " with custom allocator";

    std::cout << "\nAfter using v and s:\n";
    arena.print_free_blocks();

    std::cout << "\nvector contents: ";
    for (int x : v) std::cout << x << ' ';
    std::cout << "\nstring contents: " << s << "\n";

    return 0;
}
```

---

## Building and Running

### Requirements

- A C++17-compatible compiler

### Compile

From the project root:

```bash
g++ -std=c++17 main.cpp -o main
```

### Run

```bash
./main
```

You should see output showing:

- Free blocks before allocations (one big block).
- Free blocks after using PMR containers (fragmented into smaller blocks).
- Contents of the vector and string.

---


### Current Limitations

- **Fixed arena size**: `ByteHeapAllocator` manages a single buffer of fixed size. If it’s full, `allocate` returns `nullptr`.
- **Alignment** is currently **ignored** in `do_allocate` / `do_deallocate`. This is fine for `char` / `std::byte` and often works “by luck” in simple demos, but real-world allocators must honor alignment.
- **Heap operations are simple**:
  - `remove_free_block` rebuilds the heap with `build_max_heap`, which is O(n). This is acceptable for learning, but can be optimized.
- **Not thread-safe**: no synchronization is provided.
- **No diagnostics by default**: logging is minimal (only via `print_free_blocks()`).

---

## Possible Future Improvements

1. **Proper alignment support**
   - Pass alignment into `ByteHeapAllocator::allocate`.
   - Use `std::align` or manual alignment to carve out aligned regions.
   - Track padding metadata so deallocation is always correct.

2. **Smarter heap removal**
   - Instead of `build_max_heap` after each removal, bubble up/down from the modified index.

3. **Configurable arena size / multiple arenas**
   - Support growing the arena or chaining multiple buffers.

4. **Non-PMR allocator interface**
   - Provide a standard STL allocator (`MyAllocator<T>`) on top of `ByteHeapAllocator`, for non-PMR containers.

---

