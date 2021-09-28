add_library(utils-channel INTERFACE)
target_include_directories(utils-channel INTERFACE ${CMAKE_SOURCE_DIR})

add_executable(utils-channel-test test.cpp)
target_link_libraries(utils-channel-test PUBLIC utils-channel PRIVATE Catch2::Catch2WithMain)
add_test(NAME utils-channel COMMAND utils-channel-test)
