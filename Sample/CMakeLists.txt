project ("Sample")

set(DIRECTXMATH_DIR ../ThirdParty/DirectXMath/include)
include_directories(${DIRECTXMATH_DIR})

set(SPECTRE_COMMON_DIR ../SpectreCommon/include/)
include_directories(${SPECTRE_COMMON_DIR})

set(SPECTRE_RENDERSYSTEM_DIR ../RenderSystems/include/)
include_directories (${SPECTRE_RENDERSYSTEM_DIR})

add_subdirectory ("Sample01_Triangle")
