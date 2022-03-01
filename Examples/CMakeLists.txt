include_directories(../ThirdParty/DirectXMath/include)
include_directories(../ThirdParty/GLFW/include/)
include_directories(../Backends/include/)
include_directories(../SpectreCommon/include/)
include_directories (../SpectreCore/include/)
include_directories (../SpectreComponents/include/)


include_directories(./Base/)

set(BASE_SRC 
			 ./Base/InputType.h
			 ./Base/GLFWContext.h
			 ./Base/GLFWContext.cpp

)


add_executable (Example01_Triangle  ${BASE_SRC} "./Example01_Triangle/main.cpp")
target_link_libraries(Example01_Triangle PRIVATE SpectreCore)
target_link_libraries(Example01_Triangle PRIVATE glfw3.lib)

