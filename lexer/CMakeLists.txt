find_package(Boost COMPONENTS range fiber context system REQUIRED)
add_library(lexer STATIC src/lexer.cpp)
set_property(TARGET lexer PROPERTY CXX_STANDARD 17)
target_include_directories(lexer PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:lua>
)
add_executable(lexer-test tests/test.cpp)
target_link_libraries(lexer-test PUBLIC lexer Boost::range Boost::fiber Boost::context)
#gtest_discover_tests(lexer-test)