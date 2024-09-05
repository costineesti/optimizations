cmake_minimum_required(VERSION 3.10)
project(optimizations)

# Include additional directories
set(includeDirs
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/matplot
    ${CMAKE_CURRENT_SOURCE_DIR}/include/quadratic
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tokenize
    ${CMAKE_CURRENT_SOURCE_DIR}/include/syntax_tree
    ${CMAKE_CURRENT_SOURCE_DIR}/include/numerical
    ${CMAKE_CURRENT_SOURCE_DIR}/include/linear
)
include_directories(${includeDirs})

# Gather source files
file(GLOB_RECURSE MY_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/tokenize/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/syntax_tree/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/numerical/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/linear/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/quadratic/*.cpp"
)

# Create executable
add_executable(optimizations ${MY_SOURCE_FILES})