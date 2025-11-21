#include <iostream>
#include <vector>
#include <string>
#include <memory_resource>        // 👈 for std::pmr

#include "allocator.hpp"
#include "heap_memory_resource.hpp"

int main() {
    ByteHeapAllocator arena(1024);      // 1 KB arena
    HeapMemoryResource resource(arena); // wrap it in a PMR resource

    // PMR containers using YOUR allocator
    std::pmr::vector<int> v(&resource);
    std::pmr::basic_string<char> s(&resource); // or std::pmr::string if you prefer

    std::cout << "Initial free blocks:\n";
    arena.print_free_blocks();

    for (int i = 0; i < 10; ++i) {
        v.push_back(i * 10);
    }

    s = "hello pmr";
    s += " with custom allocator";

    std::cout << "\nAfter using v and s:\n";
    arena.print_free_blocks();

    std::cout << "\nvector contents: ";
    for (int x : v) std::cout << x << ' ';
    std::cout << "\nstring contents: " << s << "\n";

    return 0;
}
