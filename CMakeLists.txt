cmake_minimum_required(VERSION 3.16.0)
project(luacpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(
	REQUIRES
		${CONAN_EXTRA_REQUIRES}
		cmake_findboost_modular/1.69.0@bincrafters/stable
    	boost/1.73.0
	${CONAN_EXTRA_OPTIONS}
    BASIC_SETUP
    CMAKE_TARGETS
    BUILD missing
)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

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



add_executable(lua main.cpp)
set_property(TARGET lua PROPERTY CXX_STANDARD 17)
target_link_libraries(lua PUBLIC ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
