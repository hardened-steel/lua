cmake_minimum_required(VERSION 3.16.0)
project(luacpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

include(CTest)

enable_testing()

find_package(Boost 1.75.0 COMPONENTS iostreams REQUIRED)
find_package(Catch2 3 REQUIRED)

add_subdirectory(utils)
#add_subdirectory(lexer)

find_package(Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
