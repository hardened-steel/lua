cmake_minimum_required(VERSION 3.16.0)
project(luacpp VERSION 0.1.0)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

include(CTest)
enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

find_package(Boost COMPONENTS regex system date_time asio REQUIRED)

add_subdirectory(utils)
#add_subdirectory(lexer)

find_package(Threads)

add_library(${PROJECT_NAME} lua.cpp)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:lua>
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
