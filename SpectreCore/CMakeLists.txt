cmake_minimum_required (VERSION 3.8)

project ("SpectreCore")

set(INC_DIR ./include/)
set(SRC_DIR ./src/)

include_directories(${INC_DIR})

aux_source_directory(${SRC_DIR} SRC_LIST)
set(SOURCE_FILES ${SRC_LIST})


add_library (SpectreCore STATIC ${SRC_LIST})