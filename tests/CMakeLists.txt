cmake_minimum_required (VERSION 3.8)

project (unitTest)

set(INC_SPECTRECommon_DIR ../SpectreCommon/include/)

include_directories (${INC_SPECTRECommon_DIR}
    ../ThirdParty/googletest/googletest/include)

set(SPDLOG_DIR ../ThirdParty/spdlog/include)
include_directories(${SPDLOG_DIR})

IF (${CMAKE_CONFIGURATION_TYPES} MATCHES "Release")
	link_directories(../ThirdParty/spdlog/lib/x64/release
    ../ThirdParty/googletest/libs)
ELSE()
	link_directories(../ThirdParty/spdlog/lib/x64/debug
    ../ThirdParty/googletest/libs)
ENDIF()

set(SRCS ${SRC_DIR}logTest.cpp
		 ${SRC_DIR}main.cpp)

add_executable (unitTest ${SRCS})
target_link_libraries(unitTest SpectreCommon)
target_link_libraries(${PROJECT_NAME} debug gtestd optimized gtest)
