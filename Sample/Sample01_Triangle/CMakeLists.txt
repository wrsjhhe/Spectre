cmake_minimum_required (VERSION 3.8)

project (Sample01_Triangle)

include_directories (${INC_SPECTRECORE_DIR})

add_executable (Sample01_Triangle "main.cpp")

target_link_libraries(Sample01_Triangle RenderSystems)
