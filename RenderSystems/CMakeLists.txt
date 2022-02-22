cmake_minimum_required (VERSION 3.8)
project ("RenderSystems")

# ==================================================================================================
# 外部引用头文件目录设置
# ==================================================================================================

include_directories(../ThirdParty/VulkanSDK/include/)
include_directories(../)
include_directories(../SpectreCommon/include/)

# ==================================================================================================
# 项目头文件设置
# ==================================================================================================
set(INC_DIR ./include/)
include_directories(${INC_DIR})
set(PUBLIC_HDRS 
${INC_DIR}NativeWindow.h
)

set(INTERFACEE_DIR ./interface/)
include_directories(${INTERFACEE_DIR})


set(INC_VULKAN_DIR ./include/Vulkan/)
include_directories(${INC_VULKAN_DIR})
set(VULKAN_PUBLIC_HDRS 
${INC_VULKAN_DIR}RenderSystemVK.h
)

# ==================================================================================================
# 项目源文件设置
# ==================================================================================================
set(SRC_VULKAN_DIR ./src/Vulkan/)
set(VULKAN_SRCS 
${SRC_VULKAN_DIR}VulkanCommon.cpp
${SRC_VULKAN_DIR}VulkanInstance.cpp
${SRC_VULKAN_DIR}VulkanPhysicalDevice.cpp
${SRC_VULKAN_DIR}VulkanDevice.cpp
${SRC_VULKAN_DIR}VulkanQueue.cpp
${SRC_VULKAN_DIR}VulkanSwapchain.cpp
${SRC_VULKAN_DIR}VulkanBuffer.cpp
${SRC_VULKAN_DIR}VulkanCommand.cpp
${SRC_VULKAN_DIR}VulkanImages.cpp
${SRC_VULKAN_DIR}VulkanRenderPass.cpp
${SRC_VULKAN_DIR}VulkanFrameBuffer.cpp
${SRC_VULKAN_DIR}VulkanSemaphore.cpp
${SRC_VULKAN_DIR}VulkanDescriptorPool.cpp
${SRC_VULKAN_DIR}VulkanDescriptorSetLayout.cpp
${SRC_VULKAN_DIR}VulkanDescriptorSet.cpp
${SRC_VULKAN_DIR}VulkanPipeline.cpp
${SRC_VULKAN_DIR}VulkanContext.cpp
${SRC_VULKAN_DIR}VulkanIndexBuffer.cpp
${SRC_VULKAN_DIR}VulkanVertexBuffer.cpp
${SRC_VULKAN_DIR}RenderSystemVK.cpp
)

set(VULKAN_PRIVATE_HDRS
${INC_VULKAN_DIR}VulkanCommon.h
${INC_VULKAN_DIR}VulkanInstance.h
${INC_VULKAN_DIR}VulkanPhysicalDevice.h
${INC_VULKAN_DIR}VulkanSwapchain.h
${INC_VULKAN_DIR}VulkanDevice.h
${INC_VULKAN_DIR}VulkanQueue.h
${INC_VULKAN_DIR}VulkanBuffer.h
${INC_VULKAN_DIR}VulkanCommand.h
${INC_VULKAN_DIR}VulkanImages.h
${INC_VULKAN_DIR}VulkanRenderPass.h
${INC_VULKAN_DIR}VulkanFrameBuffer.h
${INC_VULKAN_DIR}VulkanSemaphore.h
${INC_VULKAN_DIR}VulkanDescriptorPool.h
${INC_VULKAN_DIR}VulkanDescriptorSetLayout.h
${INC_VULKAN_DIR}VulkanDescriptorSet.h
${INC_VULKAN_DIR}VulkanPipeline.h
${INC_VULKAN_DIR}VulkanContext.h
${INC_VULKAN_DIR}VulkanIndexBuffer.h
${INC_VULKAN_DIR}VulkanVertexBuffer.h
${INC_VULKAN_DIR}RenderSystemVK.h
)


add_library (RenderSystems STATIC ${VULKAN_PUBLIC_HDRS} ${VULKAN_PRIVATE_HDRS} ${VULKAN_SRCS})
target_link_libraries(RenderSystems PRIVATE SpectreCommon)
target_link_libraries(RenderSystems PRIVATE vulkan-1)
