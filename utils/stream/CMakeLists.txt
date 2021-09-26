add_library(utils-stream fstream.cpp)
target_include_directories(utils-stream PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(utils-stream PUBLIC Boost::iostreams)

add_executable(utils-stream-test test.cpp)
target_link_libraries(utils-stream-test PUBLIC utils-stream PRIVATE Catch2::Catch2WithMain)
add_test(NAME utils-stream COMMAND utils-stream-test)
