project ("RenderSystems")

set(VULKANSDK_DIR ../ThirdParty/VulkanSDK/)
include_directories(${VULKANSDK_DIR})

set(SPECTRE_COMMON_DIR ../SpectreCommon/)

set(INTERFACEE_DIR ./include/)

set(INC_VULKAN_DIR ./Vulkan/include/)
set(SRC_VULKAN_DIR ./Vulkan/src/)

include_directories(${SPECTRE_COMMON_DIR})
include_directories(${INTERFACEE_DIR})
include_directories(${INC_VULKAN_DIR})

set(VULKAN_SRC_FILES ${SRC_VULKAN_DIR}/VkCommon.cpp)


set(SOURCE_FILES ${VULKAN_SRC_FILES})


add_library (RenderSystems STATIC ${SOURCE_FILES})