cmake_minimum_required (VERSION 3.8)
project ("SpectreCore")


# ==================================================================================================
# 项目头文件设置
# ==================================================================================================

set(INC_DIR ./include/)
include_directories(${INC_DIR})

set(PUBLIC_HDRS ${INC_DIR}Object3D.h
				${INC_DIR}PipelineBufferManager.h
				${INC_DIR}Geometries/BufferGeometry.h
				${INC_DIR}Materials/BufferMaterial.h		
				${INC_DIR}Materials/MeshBasicMaterial.h			
				${INC_DIR}Scenes/Scene.h
				${INC_DIR}Objects/Mesh.h								
				${INC_DIR}Renderers/Renderer.h
				${INC_DIR}Cameras/Camera.h
				${INC_DIR}Cameras/PerspectiveCamera.h
				${INC_DIR}Buffers/Buffer.h
				${INC_DIR}Buffers/VertexBuffer.h
				${INC_DIR}Buffers/IndexBuffer.h

)

set(PRIVATE_HDRS )

# ==================================================================================================
# 项目源文件设置
# ==================================================================================================
set(SRC_DIR ./src/)

set(SRCS ${SRC_DIR}Object3D.cpp
		 ${SRC_DIR}PipelineBufferManager.cpp
		 ${SRC_DIR}Geometries/BufferGeometry.cpp
		 ${SRC_DIR}Materials/BufferMaterial.cpp		 
		 ${SRC_DIR}Materials/MeshBasicMaterial.cpp		 		
		 ${SRC_DIR}Scenes/Scene.cpp
		 ${SRC_DIR}Objects/Mesh.cpp
		 ${SRC_DIR}Renderers/Renderer.cpp
		 ${SRC_DIR}Cameras/Camera.cpp
		 ${SRC_DIR}Cameras/PerspectiveCamera.cpp		 
		 ${SRC_DIR}Buffers/VertexBuffer.cpp
		 ${SRC_DIR}Buffers/IndexBuffer.cpp
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
