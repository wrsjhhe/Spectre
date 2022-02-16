cmake_minimum_required (VERSION 3.8)
project ("SpectreCore")

include_directories(../ThirdParty/shaderc/include/)


include_directories (../RenderSystems/include/)
include_directories(../SpectreCommon/include/)


# ==================================================================================================
# 项目头文件设置
# ==================================================================================================

set(INC_DIR ./include/)
include_directories(${INC_DIR})

set(PUBLIC_HDRS ${INC_DIR}Engine.h
				${INC_DIR}SceneNode.h
				${INC_DIR}Scene.h
				${INC_DIR}Material.h
				${INC_DIR}ShaderTool.h
)

set(PRIVATE_HDRS )

# ==================================================================================================
# 项目源文件设置
# ==================================================================================================
set(SRC_DIR ./src/)
include_directories(${SRC_DIR})

set(SRCS ${SRC_DIR}Engine.cpp
		 ${SRC_DIR}SceneNode.cpp
		 ${SRC_DIR}Scene.cpp
		 ${SRC_DIR}Material.cpp
		 ${SRC_DIR}ShaderTool.cpp
)


add_library (SpectreCore STATIC ${PUBLIC_HDRS} ${PRIVATE_HDRS} ${SRCS} )
target_link_libraries(SpectreCore PUBLIC RenderSystems)
IF (${CMAKE_CONFIGURATION_TYPES} MATCHES "Release")
	target_link_libraries(SpectreCore PUBLIC shaderc.lib
											 shaderc_util.lib
											 glslang.lib
											 SPIRV.lib
											 MachineIndependent.lib
											 OSDependent.lib
											 OGLCompiler.lib
											 GenericCodeGen.lib
											 SPIRV-Tools-opt.lib
											 SPIRV-Tools.lib)
ELSE()
	target_link_libraries(SpectreCore PUBLIC shaderc.lib
											 shaderc_util.lib
											 glslangd.lib
											 SPIRVd.lib
											 MachineIndependentd.lib
											 OSDependentd.lib
											 OGLCompilerd.lib
											 GenericCodeGend.lib
											 SPIRV-Tools-opt.lib
											 SPIRV-Tools.lib)
ENDIF()
