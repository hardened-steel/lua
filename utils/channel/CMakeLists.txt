#channel.hpp ichannel.hpp input.hpp ochannel.hpp output.hpp transform.hpp
add_library(utils-channel INTERFACE)
target_include_directories(utils-channel INTERFACE ${CMAKE_SOURCE_DIR})
add_executable(utils-channel-test test.cpp)
target_link_libraries(utils-channel-test PUBLIC utils-channel GTest::GTest GTest::Main)
gtest_discover_tests(utils-channel-test)