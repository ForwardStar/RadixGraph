cmake_minimum_required(VERSION 3.16)
project(ForwardStar)

# Enable optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable OpenMP
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OpenMP found")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Add Spruce, Junction, and Turf directories
add_subdirectory(${CMAKE_SOURCE_DIR}/Spruce/Spruce spruce)
include_directories(${CMAKE_SOURCE_DIR}/Spruce/Spruce/junction/include)
include_directories(${CMAKE_SOURCE_DIR}/Spruce/Spruce/junction/turf/include)
include_directories(${CMAKE_SOURCE_DIR}/Spruce/turf)
include_directories(${CMAKE_SOURCE_DIR}/Spruce/turf/include)
include_directories(${CMAKE_SOURCE_DIR}/Spruce/junction)
include_directories(${CMAKE_SOURCE_DIR}/Spruce/junction/include)

add_library(FSTAR STATIC src/GAPBS/bfs.cc 
            src/GAPBS/bitmap.h 
            src/GAPBS/benchmark.h
            src/GAPBS/cc_sv.cc
            src/GAPBS/platform_atomics.h
            src/GAPBS/pr_spmv.cc
            src/GAPBS/pvector.h
            src/GAPBS/sliding_queue.h
            src/GAPBS/sssp.cc
            src/GAPBS/tc.cc
            src/forward_star.cpp 
            src/optimized_trie.cpp)

# Add executable for your main program
add_executable(forward_star
    src/main.cpp
    src/forward_star.cpp
    src/optimized_trie.cpp
    src/headers.h
    src/forward_star.h
    src/optimized_trie.h
)

add_executable(test_trie
    src/test_trie.cpp
    src/optimized_trie.cpp
    src/headers.h
    src/optimized_trie.h
)

add_executable(test_gapbs
    src/test_gapbs.cpp
    src/forward_star.cpp
    src/optimized_trie.cpp
    src/headers.h
    src/GAPBS/bfs.cc
    src/GAPBS/sssp.cc
    src/GAPBS/tc.cc
    src/GAPBS/pr_spmv.cc
    src/GAPBS/cc_sv.cc
    src/forward_star.h
    src/optimized_trie.h
    src/GAPBS/bfs.h
    src/GAPBS/benchmark.h
    src/GAPBS/sssp.h
    src/GAPBS/tc.h
    src/GAPBS/pr_spmv.h
    src/GAPBS/cc_sv.h
    src/GAPBS/platform_atomics.h
)

# Link libraries (Spruce depends on junction and turf)
target_link_libraries(forward_star PRIVATE spruce junction turf pthread OpenMP::OpenMP_CXX)
target_link_libraries(test_trie PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(test_gapbs PRIVATE OpenMP::OpenMP_CXX)