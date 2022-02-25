project ("Sample")

include_directories(../ThirdParty/DirectXMath/include)
include_directories(../ThirdParty/GLFW/include/)
include_directories(../SpectreCommon/include/)
include_directories (../SpectreCore/include/)


include_directories(./Base/)

set(Base_Src 
			 ./Base/InputType.h
			 ./Base/GLFWContext.h
			 ./Base/GLFWContext.cpp

)


add_executable (Example01_Triangle ${Base_Src} "./Example01_Triangle/main.cpp")
target_link_libraries(Example01_Triangle PRIVATE SpectreCore)
target_link_libraries(Example01_Triangle PRIVATE glfw3.lib)
