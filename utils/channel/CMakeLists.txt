add_library(utils-channel INTERFACE)
target_include_directories(utils-channel INTERFACE ${CMAKE_SOURCE_DIR})

add_library(utils-channel-test test.cpp)
target_link_libraries(utils-channel-test PUBLIC utils-channel Boost::unit_test_framework Boost::system)
add_test(NAME utils-channel COMMAND utils-channel-test)
