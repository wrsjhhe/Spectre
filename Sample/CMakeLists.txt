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

#add_subdirectory ("Sample01_Triangle")

add_executable (Sample01_Triangle ${Base_Src} "./Sample01_Triangle/main.cpp")
target_link_libraries(Sample01_Triangle PRIVATE SpectreCore)
target_link_libraries(Sample01_Triangle PRIVATE glfw3.lib)
