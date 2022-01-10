cmake_minimum_required (VERSION 3.8)
project ("SpectreCommon")

# ==================================================================================================
# 项目头文件设置
# ==================================================================================================
set(INC_DIR ./include/)
include_directories(${INC_DIR})

set(PUBLIC_HDRS ${INC_DIR}CommonDefinitions.h
				${INC_DIR}Allocator.h
				)
#set(PRIVATE_HDRS )

# ==================================================================================================
# 项目源文件设置
# ==================================================================================================
set(SRC_DIR ./src/)
include_directories(${SRC_DIR})

set(SRCS ${SRC_DIR}Allocator.cpp)

add_library (SpectreCommon STATIC ${PUBLIC_HDRS} ${PRIVATE_HDRS} ${SRCS} )
