cmake_minimum_required(VERSION 3.16)
project(ForwardStar)

# ---------------------------
# Basic C++ setup
# ---------------------------
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ---------------------------
# Find libraries
# ---------------------------
find_package(Threads REQUIRED)
find_package(TBB REQUIRED)
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OpenMP found")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# ---------------------------
# Add your RG library
# ---------------------------
add_library(RG STATIC
    src/GAPBS/bfs.cc 
    src/GAPBS/bitmap.h 
    src/GAPBS/benchmark.h
    src/GAPBS/cc_sv.cc
    src/GAPBS/platform_atomics.h
    src/GAPBS/pr_spmv.cc
    src/GAPBS/pvector.h
    src/GAPBS/sliding_queue.h
    src/GAPBS/sssp.cc
    src/GAPBS/tc.cc
    src/GAPBS/bfs.cc
    src/GAPBS/bc.cc
    src/radixgraph.cpp 
    src/sort.cpp
    src/utils.cpp
    src/vertex_array.cpp
)

# ---------------------------
# Add your executables
# ---------------------------
add_executable(test_radixgraph
    src/test_radixgraph.cpp
    src/radixgraph.cpp
    src/sort.cpp
    src/utils.cpp
    src/vertex_array.cpp
    src/utils.h
    src/radixgraph.h
    src/sort.h
    src/vertex_array.h
)

add_executable(test_trie
    src/test_trie.cpp
    src/sort.cpp
    src/utils.cpp
    src/utils.h
    src/sort.h
)

add_executable(test_transform
    src/test_transform.cpp
    src/sort.cpp
    src/utils.cpp
    src/utils.h
    src/sort.h
)

add_executable(test_gapbs
    src/test_gapbs.cpp
    src/radixgraph.cpp
    src/sort.cpp
    src/vertex_array.cpp
    src/utils.cpp
    src/utils.h
    src/vertex_array.h
    src/GAPBS/bfs.cc
    src/GAPBS/sssp.cc
    src/GAPBS/tc.cc
    src/GAPBS/pr_spmv.cc
    src/GAPBS/cc_sv.cc
    src/GAPBS/bc.cc
    src/radixgraph.h
    src/sort.h
    src/GAPBS/bfs.h
    src/GAPBS/benchmark.h
    src/GAPBS/sssp.h
    src/GAPBS/tc.h
    src/GAPBS/pr_spmv.h
    src/GAPBS/cc_sv.h
    src/GAPBS/bc.h
    src/GAPBS/platform_atomics.h
)

add_executable(test_workload
    src/test_workload.cpp
    src/radixgraph.cpp
    src/sort.cpp
    src/utils.cpp
    src/utils.h
    src/radixgraph.h
    src/sort.h
)

add_executable(test_vertex_index
    src/test_vertex_index.cpp
    src/radixgraph.cpp
    src/sort.cpp
    src/utils.cpp
    src/vertex_array.cpp
    src/utils.h
    src/radixgraph.h
    src/sort.h
    src/vertex_array.h
)

add_executable(test_transform_continuous
    src/test_transform_continuous.cpp
    src/sort.cpp
    src/utils.cpp
    src/utils.h
    src/sort.h
)

add_executable(test_analytics
    src/test_analytics.cpp
    src/radixgraph.cpp
    src/sort.cpp
    src/vertex_array.cpp
    src/utils.cpp
    src/utils.h
    src/radixgraph.h
    src/sort.h
    src/vertex_array.h
    src/GAPBS/bfs.cc
    src/GAPBS/sssp.cc
    src/GAPBS/tc.cc
    src/GAPBS/pr_spmv.cc
    src/GAPBS/cc_sv.cc
    src/GAPBS/bc.cc
    src/GAPBS/bfs.h
    src/GAPBS/benchmark.h
    src/GAPBS/sssp.h
    src/GAPBS/tc.h
    src/GAPBS/pr_spmv.h
    src/GAPBS/cc_sv.h
    src/GAPBS/bc.h
    src/GAPBS/platform_atomics.h
)

add_executable(test_batch_updates
    src/test_batch_updates.cpp
    src/radixgraph.cpp
    src/sort.cpp
    src/vertex_array.cpp
    src/utils.cpp
    src/utils.h
    src/radixgraph.h
    src/sort.h
    src/vertex_array.h
)

# ---------------------------
# Link libraries
# ---------------------------
foreach(target test_radixgraph test_trie test_transform test_gapbs test_workload test_vertex_index test_transform_continuous test_analytics test_batch_updates)
    target_link_libraries(${target} PRIVATE Threads::Threads OpenMP::OpenMP_CXX TBB::tbb RG)
endforeach()

# ---------------------------
# Optional: UnoDB integration
# ---------------------------
set(UNODB_PATH "${CMAKE_SOURCE_DIR}/unodb")
if(EXISTS "${UNODB_PATH}/CMakeLists.txt")
    message(STATUS "UnoDB detected at ${UNODB_PATH}")

    # Configure UnoDB
    set(SPINLOCK_LOOP "PAUSE" CACHE STRING "Spinlock loop type")
    set(UNODB_BUILD_TESTS OFF CACHE BOOL "Disable tests")
    set(UNODB_BUILD_STATS OFF CACHE BOOL "Disable stats")

    # Add UnoDB subdirectory
    add_subdirectory(${UNODB_PATH})

    # Include UnoDB headers
    target_include_directories(RG PRIVATE ${UNODB_PATH})
    foreach(target test_radixgraph test_trie test_transform test_gapbs test_workload test_vertex_index test_analytics test_batch_updates)
        target_include_directories(${target} PRIVATE ${UNODB_PATH})
        target_link_libraries(${target} PRIVATE unodb)
    endforeach()

    # Add ART source to RG only when UnoDB is available
    target_sources(RG PRIVATE src/art.cpp)

    # Enable SIMD flags for your CPU
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(RG PRIVATE -march=native -O3)
        foreach(target test_radixgraph test_trie test_transform test_gapbs test_workload test_vertex_index test_analytics test_batch_updates)
            target_compile_options(${target} PRIVATE -march=native -O3)
        endforeach()
    endif()
else()
    message(STATUS "UnoDB not found — building without UnoDB")
endif()
