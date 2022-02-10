cmake_minimum_required (VERSION 3.8)
project ("SpectreCore")

set(SPECTRE_RENDERSYSTEM_DIR ../RenderSystems/include/)
include_directories (${SPECTRE_RENDERSYSTEM_DIR})

set(SPECTRE_COMMON_DIR ../SpectreCommon/include/)
include_directories(${SPECTRE_COMMON_DIR})
# ==================================================================================================
# 项目头文件设置
# ==================================================================================================

set(INC_DIR ./include/)
include_directories(${INC_DIR})

set(PUBLIC_HDRS ${INC_DIR}Engine.h)

set(PRIVATE_HDRS )

# ==================================================================================================
# 项目源文件设置
# ==================================================================================================
set(SRC_DIR ./src/)
include_directories(${SRC_DIR})

set(SRCS ${SRC_DIR}Engine.cpp)


add_library (SpectreCore STATIC ${PUBLIC_HDRS} ${PRIVATE_HDRS} ${SRCS} )
target_link_libraries(SpectreCore PUBLIC RenderSystems)
