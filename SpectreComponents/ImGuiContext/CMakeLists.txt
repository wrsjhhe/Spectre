project ("ImGuiContext")

include_directories("../../ThirdParty/VulkanSDK/include/")

set(IMGUI_SRC_DIR ../../ThirdParty/imgui/)
include_directories(${IMGUI_SRC_DIR})


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

add_executable (ImGuiContext "ImGuiContext.h" "ImGuiContext.cpp")