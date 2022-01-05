project ("RenderSystems")

set(INTERFACEE_DIR ./include/)

set(INC_VULKAN_DIR ./vulkan/include)
set(SRC_VULKAN_DIR ./vulkan/src/)

include_directories(${INTERFACEE_DIR})
include_directories(${INC_VULKAN_DIR})
aux_source_directory(${SRC_VULKAN_DIR} SRC_LIST)
set(SOURCE_FILES ${SRC_LIST})


add_library (RENDERSYSTEM STATIC ${SOURCE_FILES})