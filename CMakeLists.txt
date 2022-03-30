# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("Spectre")

add_compile_definitions(RESOURCES_DIR=\"${CMAKE_SOURCE_DIR}/Resources/\")
add_compile_definitions(_HAS_CXX17=1)
add_compile_definitions(_HAS_CXX20=0)

if (MSVC)
    set(CXX_STANDARD "/std:c++17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_STANDARD} /W0 /Zc:__cplusplus  /EHsc ")
else()
    set(CXX_STANDARD "-std=c++17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_STANDARD} -fstrict-aliasing -Wno-unknown-pragmas -Wno-unused-function")
endif()

#ThirdParty
IF (${CMAKE_CONFIGURATION_TYPES} MATCHES "Release")
	link_directories(./ThirdParty/spdlog/lib/x64/release)
	link_directories(./ThirdParty/shaderc/lib/rls)
ELSE()
	link_directories(./ThirdParty/spdlog/lib/x64/debug)
	link_directories(./ThirdParty/shaderc/lib/dbg)
ENDIF()
link_directories(./ThirdParty/VulkanSDK/lib)
link_directories(./ThirdParty/GLFW/lib)
include_directories(./ThirdParty/spdlog/include)
include_directories(./ThirdParty/VulkanSDK/include/)
include_directories(./ThirdParty/imgui/)

include_directories (./Backends/include/)
include_directories (./SpectreCommon/include/)
# 包含子项目。
add_subdirectory ("SpectreCommon")
add_subdirectory ("Backends")
add_subdirectory ("SpectreCore")
add_subdirectory ("Examples")
#add_subdirectory ("tests")
