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

set(PUBLIC_HDRS ${INC_DIR}SpectreDef.h
				${INC_DIR}Allocator.h
				${INC_DIR}Platform.h
				${INC_DIR}Mutex.h
				${INC_DIR}FormotString.h
				${INC_DIR}LogManager.h
				${INC_DIR}Noncopyable.h
				${INC_DIR}NativeWindow.h
				${INC_DIR}Timer.h
				${INC_DIR}Math/MathDef.h
				${INC_DIR}Math/SimpleMath.h
				${INC_DIR}Math/SimpleMath.inl
				)
#set(PRIVATE_HDRS )

# ==================================================================================================
# 项目源文件设置
# ==================================================================================================
set(SRC_DIR ./src/)
include_directories(${SRC_DIR})

set(SRCS ${SRC_DIR}Allocator.cpp
		 ${SRC_DIR}Mutex.cpp
		 ${SRC_DIR}LogManager.cpp
		 ${SRC_DIR}Timer.cpp
		 ${SRC_DIR}Math/SimpleMath.cpp
)


add_library (SpectreCommon STATIC ${PUBLIC_HDRS} ${PRIVATE_HDRS} ${SRCS})
target_link_libraries(SpectreCommon PRIVATE spdlog)
