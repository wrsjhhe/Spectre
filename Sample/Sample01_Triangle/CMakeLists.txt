cmake_minimum_required (VERSION 3.8)

project (Sample01_Triangle)

add_executable (Sample01_Triangle "main.cpp")

target_link_libraries(Sample01_Triangle PRIVATE SpectreCore)
target_link_libraries(Sample01_Triangle PRIVATE glfw3.lib)