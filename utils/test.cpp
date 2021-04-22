#include <gtest/gtest.h>
#include <iostream>

#include <utils/regular.hpp>
#include <utils/tokenizer.hpp>

#include <cstdlib>
#include <memory>
#include <cxxabi.h>

namespace {
	std::string demangle(const char* name) {

	    int status = -4; // some arbitrary value to eliminate the compiler warning
	    return abi::__cxa_demangle(name, NULL, NULL, &status);
	}
}


TEST(utils, regex)
{
	using namespace utils;
	EXPECT_TRUE(match("abc")("abc11"));
	EXPECT_FALSE(match("abc")("ab"));
	EXPECT_TRUE((match("abc") | match("cba"))("abc11"));
	EXPECT_TRUE((match("abc") | match("cba"))("cba22"));
	EXPECT_FALSE((match("abc") | match("cba"))("def"));
	EXPECT_TRUE((*match("a"))("aaa"));
	{
		auto regex = match("ab") & !match("c") & match("d");
		EXPECT_TRUE(regex("abd"));
		EXPECT_TRUE(regex("abcd"));
		EXPECT_FALSE(regex("abed"));
	}
	{
		auto regex = match("ab") & +match("c") & match("d");
		EXPECT_FALSE(regex("abd"));
		EXPECT_TRUE(regex("abcd"));
		EXPECT_TRUE(regex("abccd"));
		EXPECT_TRUE(regex("abcccde"));
		EXPECT_FALSE(regex("abce"));
	}
	{
		auto regex = match["abcdef"];
		EXPECT_TRUE(regex("a111"));
		EXPECT_TRUE(regex("b222"));
		EXPECT_FALSE(regex("g333"));
	}
	{
		auto symbol = match('a', 'z') | match('A', 'Z') | match("_");
		auto number = match["1234567890"];
		auto regex = symbol & *(symbol | number);
		EXPECT_TRUE(regex("some_value"));
		EXPECT_TRUE(regex("_"));
		EXPECT_TRUE(regex("LegalIdentifier"));
		EXPECT_FALSE(regex("122_bytes"));
		EXPECT_FALSE(regex(" value"));
	}
}

TEST(utils, tokenizer)
{
	const auto& match = utils::match;
	auto symbol = match('a', 'z') | match('A', 'Z') | match["_"];
	auto number = match('0', '9');
	auto identifier = symbol & *(symbol | number);
	auto token = identifier >> [](const char* b, const char* e) {
		return std::string("identifier{\"") + std::string(b, e)  + "\"}";
	};
	EXPECT_EQ(token("abcd"), std::string(R"(identifier{"abcd"})"));
}

TEST(utils, lexer)
{
	const auto& match = utils::match;
	auto symbol = match('a', 'z') | match('A', 'Z') | match["_"];
	auto number = match('0', '9');
	auto identifier = symbol & *(symbol | number);
	auto lexer = utils::tokenize(
		utils::tokens(
			identifier >> [](const char* b, const char* e) {
				return std::string("identifier{\"") + std::string(b, e) + "\"}";
			},
			+number >> [](const char* b, const char* e) {
				return std::string("number{" + std::string(b, e) + "}");
			}
		),
		utils::channel::input("")
	);
	//std::cout << tokenize(lexer)("abcd") << std::endl;
	//std::cout << tokenize(lexer)("1234") << std::endl;
}
