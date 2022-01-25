cmake_minimum_required (VERSION 3.8)

project (Sample00_HelloWorld)

set(INC_RENDERSYSTEM_DIR ../../RenderSystems/include/)
include_directories (${INC_RENDERSYSTEM_DIR})

add_executable (Sample00_HelloWorld "main.cpp")

target_link_libraries(Sample00_HelloWorld RenderSystems)
