add_library(utils-regex INTERFACE)
target_include_directories(utils-regex INTERFACE ${CMAKE_SOURCE_DIR})

add_executable(utils-regex-test test.cpp)
target_link_libraries(utils-regex-test PUBLIC utils-regex PRIVATE Catch2::Catch2WithMain)
add_test(NAME utils-regex COMMAND utils-regex-test)
