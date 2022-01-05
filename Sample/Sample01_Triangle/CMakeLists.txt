cmake_minimum_required (VERSION 3.8)
add_definitions(-std=c++17)

project (Sample01_Triangle)

set(INC_SPECTRECORE_DIR ../../SpectreCore/include/)

include_directories (${INC_SPECTRECORE_DIR})

add_executable (Sample "main.cpp")

target_link_libraries(Sample SpectreCore)
