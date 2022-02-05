project ("Sample")

set(VULKANSDK_DIR ../ThirdParty/VulkanSDK/include/)
include_directories(${VULKANSDK_DIR})

add_subdirectory ("Sample00_HelloWorld")
#add_subdirectory ("Sample01_Triangle")
