#include <zobarcik/heap.h>
#include <memory_resource>
#include <iostream>

int main() {
    std::byte arena[2048];
    std::pmr::monotonic_buffer_resource rsrc(arena, sizeof(arena));
    zobarcik::heap_pmr<int> h{std::less<int>{}, std::pmr::polymorphic_allocator<int>{&rsrc}};
    h.push(11); h.push(3); h.push(27);
    std::cout << h.top() << "\n"; // 27
}

/*
mkdir build
cd build
cmake ..
cmake --build .
./my_app
*/