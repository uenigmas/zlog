cmake_minimum_required(VERSION 3.0.0)
project(zlog VERSION 0.1.0)

include(CTest)
enable_testing()

aux_source_directory(../ DIR_SRCS)

if(CMAKE_SYSTEM_NAME MATCHES "Linux|FreeBSD")
	link_libraries(pthread)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

add_executable(zlog main.cpp ${DIR_SRCS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
