cmake_minimum_required(VERSION 3.10)

# Set the project name
project(ForwardStar)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the source files
set(SOURCES
    src/main.cpp
    src/adj_list.cpp
    src/forward_star.cpp
)

# Add the header files (optional, not required by CMake but useful for IDEs)
set(HEADERS
    src/adj_list.h
    src/forward_star.h
    src/headers.h
)

# Create the executable
add_executable(ForwardStar ${SOURCES} ${HEADERS})
