add_subdirectory(channel)
add_subdirectory(stream)

#add_library(utils test.cpp)
#target_include_directories(utils-stream PUBLIC ${CMAKE_SOURCE_DIR})

#add_executable(utils-test test.cpp)
#target_include_directories(utils-test PUBLIC ${CMAKE_SOURCE_DIR})
#target_link_libraries(utils-test PUBLIC GTest::GTest GTest::Main)
#gtest_discover_tests(utils-test)
