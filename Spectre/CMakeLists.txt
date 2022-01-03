cmake_minimum_required (VERSION 3.8)

project ("Spectre")

set(INC_SPECTRE_DIR ./include/)
set(SRC_SPECTRE_DIR ./src/)

include_directories(${INC_SPECTRE_DIR})

aux_source_directory(${SRC_SPECTRE_DIR} SRC_LIST)
set(SOURCE_FILES ${SRC_LIST})


add_library (Spectre STATIC ${SRC_LIST})