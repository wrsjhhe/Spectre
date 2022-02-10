# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("Spectre")

if (WIN32)
    set(CXX_STANDARD "/std:c++17")
else()
    set(CXX_STANDARD "-std=c++17")
endif()

if (MSVC)
    set(CXX_STANDARD "/std:c++latest")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_STANDARD} /W0 /Zc:__cplusplus")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_STANDARD} -fstrict-aliasing -Wno-unknown-pragmas -Wno-unused-function")
endif()

IF (${CMAKE_CONFIGURATION_TYPES} MATCHES "Release")
	link_directories(./ThirdParty/spdlog/lib/x64/release)
ELSE()
	link_directories(./ThirdParty/spdlog/lib/x64/debug)
ENDIF()
link_directories(./ThirdParty/VulkanSDK/lib)
link_directories(./ThirdParty/GLFW/lib)

# 包含子项目。
add_subdirectory ("SpectreCommon")
add_subdirectory ("RenderSystems")
add_subdirectory ("SpectreCore")
add_subdirectory ("Sample")
#add_subdirectory ("tests")
