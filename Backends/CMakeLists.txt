project ("Backends")

# ==================================================================================================
# 外部引用头文件目录设置
# ==================================================================================================

set(IMGUI_SRC_DIR ../ThirdParty/imgui/)

set(IMGUI_SRC 
			    ${IMGUI_SRC_DIR}imconfig.h
				${IMGUI_SRC_DIR}imgui.h
				${IMGUI_SRC_DIR}imgui_internal.h
				${IMGUI_SRC_DIR}imstb_rectpack.h
				${IMGUI_SRC_DIR}imstb_textedit.h
				${IMGUI_SRC_DIR}imstb_truetype.h
				${IMGUI_SRC_DIR}imgui.cpp
				${IMGUI_SRC_DIR}imgui_draw.cpp
				${IMGUI_SRC_DIR}imgui_widgets.cpp
				${IMGUI_SRC_DIR}imgui_tables.cpp
				${IMGUI_SRC_DIR}imgui_impl_vulkan.cpp
				${IMGUI_SRC_DIR}imgui_impl_vulkan.cpp
)

include_directories(../SpectreCommon/include/)

# ==================================================================================================
# 项目头文件设置
# ==================================================================================================
set(INC_DIR ./include/)
include_directories(${INC_DIR})
set(PUBLIC_HDRS 
)

set(INTERFACEE_DIR ./interface/)
include_directories(${INTERFACEE_DIR})


set(INC_VULKAN_DIR ./include/)
include_directories(${INC_VULKAN_DIR})
set(VULKAN_PUBLIC_HDRS 
${INC_VULKAN_DIR}RenderSystemVK.h
)

# ==================================================================================================
# 项目源文件设置
# ==================================================================================================
set(SRC_VULKAN_DIR ./src/)
set(VULKAN_SRCS 
${SRC_VULKAN_DIR}VulkanCommon.cpp
${SRC_VULKAN_DIR}VulkanEngine.cpp
${SRC_VULKAN_DIR}VulkanSwapchain.cpp
${SRC_VULKAN_DIR}VulkanBuffer.cpp
${SRC_VULKAN_DIR}VulkanCommand.cpp
${SRC_VULKAN_DIR}VulkanImages.cpp
${SRC_VULKAN_DIR}VulkanRenderPass.cpp
${SRC_VULKAN_DIR}VulkanFrameBuffer.cpp
${SRC_VULKAN_DIR}VulkanSemaphore.cpp
${SRC_VULKAN_DIR}VulkanPipeline.cpp
${SRC_VULKAN_DIR}VulkanContext.cpp

${SRC_VULKAN_DIR}VulkanGui.cpp
${SRC_VULKAN_DIR}RenderSystemVK.cpp
)

set(VULKAN_PRIVATE_HDRS
${INC_VULKAN_DIR}VulkanCommon.h
${INC_VULKAN_DIR}VulkanEngine.h
${INC_VULKAN_DIR}VulkanSwapchain.h
${INC_VULKAN_DIR}VulkanBuffer.h
${INC_VULKAN_DIR}VulkanCommand.h
${INC_VULKAN_DIR}VulkanImages.h
${INC_VULKAN_DIR}VulkanRenderPass.h
${INC_VULKAN_DIR}VulkanFrameBuffer.h
${INC_VULKAN_DIR}VulkanSemaphore.h
${INC_VULKAN_DIR}VulkanPipeline.h
${INC_VULKAN_DIR}VulkanContext.h

${INC_VULKAN_DIR}VulkanGui.h
${INC_VULKAN_DIR}RenderSystemVK.h
)


add_library (Backends STATIC ${PUBLIC_HDRS} ${VULKAN_PUBLIC_HDRS} ${VULKAN_PRIVATE_HDRS} ${VULKAN_SRCS} ${IMGUI_SRC})
target_link_libraries(Backends PUBLIC SpectreCommon)
target_link_libraries(Backends PUBLIC vulkan-1)
