cmake_minimum_required (VERSION 3.8)
project ("SpectreCore")

include_directories(../ThirdParty/shaderc/include/)

include_directories (../Backends/include/)
include_directories(../SpectreCommon/include/)


# ==================================================================================================
# 项目头文件设置
# ==================================================================================================

set(INC_DIR ./include/)
include_directories(${INC_DIR})

set(PUBLIC_HDRS 
				${INC_DIR}Object.h
				${INC_DIR}ShaderTool.h
				${INC_DIR}Geometries/BufferGeometry.h
				${INC_DIR}Materials/Material.h				
				${INC_DIR}Objects/Object3D.h
				${INC_DIR}Objects/Scene.h
				${INC_DIR}Objects/Mesh.h								
				${INC_DIR}Renderers/Renderer.h
				${INC_DIR}Cameras/Camera.h
				${INC_DIR}Cameras/PerspectiveCamera.h

)

set(PRIVATE_HDRS )

# ==================================================================================================
# 项目源文件设置
# ==================================================================================================
set(SRC_DIR ./src/)

set(SRCS 
		 ${SRC_DIR}Object.cpp
		 ${SRC_DIR}ShaderTool.cpp		 
		 ${SRC_DIR}Geometries/BufferGeometry.cpp
		 ${SRC_DIR}Materials/Material.cpp		 
		 ${SRC_DIR}Objects/Object3D.cpp
		 ${SRC_DIR}Objects/Scene.cpp
		 ${SRC_DIR}Objects/Mesh.cpp
		 ${SRC_DIR}Renderers/Renderer.cpp
		 ${SRC_DIR}Cameras/Camera.cpp
		 ${SRC_DIR}Cameras/PerspectiveCamera.cpp
)


add_library (SpectreCore STATIC ${PUBLIC_HDRS} ${PRIVATE_HDRS} ${SRCS} )
target_link_libraries(SpectreCore PUBLIC Backends)
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
