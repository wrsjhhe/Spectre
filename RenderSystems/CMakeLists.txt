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

set(PUBLIC_HDRS ${INC_VULKAN_DIR}VulkanInstance.h)

# ==================================================================================================
# 项目源文件设置
# ==================================================================================================
set(SRC_VULKAN_DIR ./Vulkan/src/)
set(VULKAN_SRCS ${SRC_VULKAN_DIR}VkCommon.cpp)


add_library (RenderSystems STATIC ${PUBLIC_HDRS} ${VULKAN_SRCS})
target_link_libraries(RenderSystems PRIVATE SpectreCommon)