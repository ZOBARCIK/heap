cmake_minimum_required(VERSION 3.16)
project(zobarcik-heap LANGUAGES CXX VERSION 0.1.0)

# 1) Header-only target
add_library(zobarcik_heap INTERFACE)
add_library(zobarcik::zobarcik_heap ALIAS zobarcik_heap)

# 2) Public include path (tells consumers where heap.hpp is)
target_include_directories(zobarcik_heap INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE zobarcik_heap)


# 3) Require a C++ standard for consumers
target_compile_features(zobarcik_heap INTERFACE cxx_std_17)

# 4) Install headers (copies the whole include/ tree)
include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# 5) Export the target so find_package() works
install(TARGETS zobarcik_heap
        EXPORT zobarcik_heapTargets)

install(EXPORT zobarcik_heapTargets
        FILE zobarcik_heapTargets.cmake
        NAMESPACE zobarcik::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zobarcik_heap)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/zobarcik_heapConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

# Minimal config file that just includes the exported targets
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/zobarcik_heapConfig.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/zobarcik_heapTargets.cmake\")\n")

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/zobarcik_heapConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zobarcik_heapConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zobarcik_heap)

