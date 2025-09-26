cmake_minimum_required(VERSION 3.16)
project(ForwardStar)

# Enable optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find pthreads
find_package(Threads REQUIRED)

find_package(TBB REQUIRED)  # Find TBB

# Enable OpenMP
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OpenMP found")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_library(RG STATIC src/GAPBS/bfs.cc 
            src/GAPBS/bitmap.h 
            src/GAPBS/benchmark.h
            src/GAPBS/cc_sv.cc
            src/GAPBS/platform_atomics.h
            src/GAPBS/pr_spmv.cc
            src/GAPBS/pvector.h
            src/GAPBS/sliding_queue.h
            src/GAPBS/sssp.cc
            src/GAPBS/tc.cc
            src/radixgraph.cpp 
            src/optimized_trie.cpp)

# Add executable for your main program
add_executable(test_radixgraph
    src/main.cpp
    src/radixgraph.cpp
    src/optimized_trie.cpp
    src/headers.h
    src/radixgraph.h
    src/optimized_trie.h
)

add_executable(test_trie
    src/test_trie.cpp
    src/optimized_trie.cpp
    src/headers.h
    src/optimized_trie.h
)

add_executable(test_transform
    src/test_transform.cpp
    src/optimized_trie.cpp
    src/headers.h
    src/optimized_trie.h
)

add_executable(test_gapbs
    src/test_gapbs.cpp
    src/radixgraph.cpp
    src/optimized_trie.cpp
    src/headers.h
    src/GAPBS/bfs.cc
    src/GAPBS/sssp.cc
    src/GAPBS/tc.cc
    src/GAPBS/pr_spmv.cc
    src/GAPBS/cc_sv.cc
    src/radixgraph.h
    src/optimized_trie.h
    src/GAPBS/bfs.h
    src/GAPBS/benchmark.h
    src/GAPBS/sssp.h
    src/GAPBS/tc.h
    src/GAPBS/pr_spmv.h
    src/GAPBS/cc_sv.h
    src/GAPBS/platform_atomics.h
)

add_executable(test_workload
    src/test_workload.cpp
    src/radixgraph.cpp
    src/optimized_trie.cpp
    src/headers.h
    src/radixgraph.h
    src/optimized_trie.h
)

# Link libraries
target_link_libraries(test_radixgraph PRIVATE Threads::Threads OpenMP::OpenMP_CXX TBB::tbb)
target_link_libraries(test_trie PRIVATE Threads::Threads OpenMP::OpenMP_CXX TBB::tbb)
target_link_libraries(test_transform PRIVATE Threads::Threads OpenMP::OpenMP_CXX TBB::tbb)
target_link_libraries(test_gapbs PRIVATE Threads::Threads OpenMP::OpenMP_CXX TBB::tbb)
target_link_libraries(test_workload PRIVATE Threads::Threads OpenMP::OpenMP_CXX TBB::tbb)