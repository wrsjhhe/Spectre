cmake_minimum_required (VERSION 3.8)
#add_definitions(-std=c++17)
set(CMAKE_CXX_FLAGS"${CMAKE_CXX_FLAGS} /std:c++17")
project ("Backend")

set(INC_BACKEND_DIR ./include/)
set(SRC_BACKEND_DIR ./src/)

include_directories(${INC_BACKEND_DIR})
aux_source_directory(${SRC_BACKEND_DIR} SRC_BACKEND_LIST)
set(SOURCE_FILES ${SRC_BACKEND_LIST})


add_library (Backend STATIC ${SOURCE_FILES})