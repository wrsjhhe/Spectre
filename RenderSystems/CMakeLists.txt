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
set(INTERFACEE_DIR ./interface/)
set(INC_VULKAN_DIR ./include/Vulkan/)

include_directories(${INTERFACEE_DIR})
include_directories(${INC_VULKAN_DIR})

set(VULKAN_PUBLIC_HDRS ${INC_VULKAN_DIR}RenderSystemVK.h
)

# ==================================================================================================
# 项目源文件设置
# ==================================================================================================
set(SRC_VULKAN_DIR ./src/Vulkan/)
set(VULKAN_SRCS 
${SRC_VULKAN_DIR}VulkanCommon.cpp
${SRC_VULKAN_DIR}VulkanInstance.cpp
${SRC_VULKAN_DIR}VulkanPhysicalDevice.cpp
${SRC_VULKAN_DIR}VulkanLogicalDevice.cpp
${SRC_VULKAN_DIR}RenderSystemVK.cpp
)

set(VULKAN_PRIVATE_HDRS
${INC_VULKAN_DIR}VulkanCommon.h
${INC_VULKAN_DIR}VulkanInstance.h
${INC_VULKAN_DIR}VulkanPhysicalDevice.h
${INC_VULKAN_DIR}VulkanLogicalDevice.h
)

add_library (RenderSystems STATIC ${VULKAN_PUBLIC_HDRS} ${VULKAN_PRIVATE_HDRS} ${VULKAN_SRCS})
target_link_libraries(RenderSystems PRIVATE SpectreCommon)