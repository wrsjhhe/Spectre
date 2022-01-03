cmake_minimum_required (VERSION 3.8)
add_definitions(-std=c++17)

project ("Sample")

set(INC_SPECTRE_DIR ../Spectre/include/)

include_directories (${INC_SPECTRE_DIR})

add_executable (Sample "main.cpp")

target_link_libraries(Sample Spectre)
