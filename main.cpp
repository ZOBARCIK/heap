#include "allocator.hpp"
#include <iostream>
#include <cstring> // for std::memset

int main() {


    ByteHeapAllocator alloc(1024); // 1 KB

    std::cout << "Initial state:\n";
    alloc.print_free_blocks();

    void* p1 = alloc.allocate(128);
    void* p2 = alloc.allocate(256);

    std::cout << "p1=" << p1 << "\n";
    std::cout << "p2=" << p2 << "\n";

    // use p1 as an int array
    int* a = static_cast<int*>(p1);
    for (int i = 0; i < 128 / sizeof(int); ++i) {
        a[i] = i;
    }

    std::cout << "a[0]=" << a[0] << " a[5]=" << a[5] << "\n";

    // use p2 as raw bytes
    unsigned char* b = static_cast<unsigned char*>(p2);
    for (int i = 0; i < 10; ++i) {
        b[i] = 42; // just some value
    }
    std::cout << "b[0]=" << static_cast<int>(b[0]) << "\n";

    alloc.deallocate(p1, 128);
    alloc.deallocate(p2, 256);


    std::cout << "Last state:\n";
    alloc.print_free_blocks();

    return 0;
}
