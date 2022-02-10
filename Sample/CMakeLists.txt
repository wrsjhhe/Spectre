project ("Sample")

set(DIRECTXMATH_DIR ../ThirdParty/DirectXMath/include)
include_directories(${DIRECTXMATH_DIR})

set(GLFW_DIR ../ThirdParty/GLFW/include/)
include_directories(${GLFW_DIR})

set(SPECTRE_COMMON_DIR ../SpectreCommon/include/)
include_directories(${SPECTRE_COMMON_DIR})

set(SPECTRE_CORE_DIR ../SpectreCore/include/)
include_directories (${SPECTRE_CORE_DIR})


add_subdirectory ("Sample01_Triangle")
