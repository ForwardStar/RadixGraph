cmake_minimum_required(VERSION 3.16)
project(ForwardStar)

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

# Include directories for external libraries if needed (adjust paths as necessary)
include_directories("/mnt/wd_ultrastar_8TB/sjf/GraphIndex/Comparisons/teseo-master/include")
include_directories("/usr/local/include/sdsl")
include_directories("/usr/local/include/tbb/")
include_directories("/usr/local/libevent/2_1_12/include/")

# Add Spruce, Junction, and Turf directories
add_subdirectory(${CMAKE_SOURCE_DIR}/Spruce/Spruce spruce)
include_directories(${CMAKE_SOURCE_DIR}/Spruce/Spruce/junction/include)
include_directories(${CMAKE_SOURCE_DIR}/Spruce/Spruce/junction/turf/include)
include_directories(${CMAKE_SOURCE_DIR}/Spruce/turf)
include_directories(${CMAKE_SOURCE_DIR}/Spruce/turf/include)
include_directories(${CMAKE_SOURCE_DIR}/Spruce/junction)
include_directories(${CMAKE_SOURCE_DIR}/Spruce/junction/include)

# Add executable for your main program
add_executable(forward_star 
    src/main.cpp
    src/adj_list.cpp
    src/forward_star.cpp
    src/headers.h
    src/adj_list.h
    src/forward_star.h
)

# Link libraries (Spruce depends on junction and turf)
target_link_libraries(forward_star PRIVATE spruce junction turf pthread OpenMP::OpenMP_CXX)
