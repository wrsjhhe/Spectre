cmake_minimum_required (VERSION 3.8)
project ("SpectreCommon")

# ==================================================================================================
# 项目头文件设置
# ==================================================================================================
set(INC_DIR ./include/)
include_directories(${INC_DIR})

set(SPDLOG_DIR ../ThirdParty/spdlog/include)
include_directories(${SPDLOG_DIR})

set(DIRECTXMATH_DIR ../ThirdParty/DirectXMath/include)
include_directories(${DIRECTXMATH_DIR})

set(PUBLIC_HDRS ${INC_DIR}CommonDefinitions.h
				${INC_DIR}Allocator.h
				${INC_DIR}Platform.h
				${INC_DIR}Mutex.h
				${INC_DIR}FormotString.h
				${INC_DIR}LogManager.h
				)
#set(PRIVATE_HDRS )

# ==================================================================================================
# 项目源文件设置
# ==================================================================================================
set(SRC_DIR ./src/)
include_directories(${SRC_DIR})

set(SRCS ${SRC_DIR}Allocator.cpp
		 ${SRC_DIR}Mutex.cpp
		 ${SRC_DIR}LogManager.cpp)


add_library (SpectreCommon STATIC ${PUBLIC_HDRS} ${PRIVATE_HDRS} ${SRCS})
target_link_libraries(SpectreCommon PRIVATE spdlog)
