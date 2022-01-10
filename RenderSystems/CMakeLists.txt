cmake_minimum_required (VERSION 3.8)
project ("RenderSystems")

# ==================================================================================================
# 外部引用头文件目录设置
# ==================================================================================================
set(VULKANSDK_DIR ../ThirdParty/VulkanSDK/)
include_directories(${VULKANSDK_DIR})

set(SPECTRE_COMMON_DIR ../SpectreCommon/include/)
include_directories(${SPECTRE_COMMON_DIR})

# ==================================================================================================
# 项目头文件设置
# ==================================================================================================
set(INTERFACEE_DIR ./include/)
set(INC_VULKAN_DIR ./Vulkan/include/)

include_directories(${INTERFACEE_DIR})
include_directories(${INC_VULKAN_DIR})

# ==================================================================================================
# 项目源文件设置
# ==================================================================================================
set(SRC_VULKAN_DIR ./Vulkan/src/)
set(VULKAN_SRC_FILES ${SRC_VULKAN_DIR}/VkCommon.cpp)

set(SOURCE_FILES ${VULKAN_SRC_FILES})


add_library (RenderSystems STATIC ${SOURCE_FILES})