#define BOOST_TEST_MODULE utils regex unit test
#include <boost/test/included/unit_test.hpp>

#include <utils/regex/regular.hpp>
#include <utils/regex/tokenizer.hpp>

BOOST_AUTO_TEST_SUITE(regex)

BOOST_AUTO_TEST_CASE(base)
{
	using namespace utils;
	BOOST_TEST(match("abc")("abc11"));
	BOOST_TEST(!match("abc")("ab"));
	BOOST_TEST((match("abc") | match("cba"))("abc11"));
	BOOST_TEST((match("abc") | match("cba"))("cba22"));
	BOOST_TEST(!(match("abc") | match("cba"))("def"));
	BOOST_TEST((*match("a"))("aaa"));
	{
		auto regex = match("ab") & !match("c") & match("d");
		BOOST_TEST(regex("abd"));
		BOOST_TEST(regex("abcd"));
		BOOST_TEST(!regex("abed"));
	}
	{
		auto regex = match("ab") & +match("c") & match("d");
		BOOST_TEST(!regex("abd"));
		BOOST_TEST(regex("abcd"));
		BOOST_TEST(regex("abccd"));
		BOOST_TEST(regex("abcccde"));
		BOOST_TEST(!regex("abce"));
	}
	{
		auto regex = match["abcdef"];
		BOOST_TEST(regex("a111"));
		BOOST_TEST(regex("b222"));
		BOOST_TEST(!regex("g333"));
	}
	{
		auto symbol = match('a', 'z') | match('A', 'Z') | match("_");
		auto number = match["1234567890"];
		auto regex = symbol & *(symbol | number);
		BOOST_TEST(regex("some_value"));
		BOOST_TEST(regex("_"));
		BOOST_TEST(regex("LegalIdentifier"));
		BOOST_TEST(!regex("122_bytes"));
		BOOST_TEST(!regex(" value"));
	}
}

BOOST_AUTO_TEST_CASE(tokenizer)
{
	const auto& match = utils::match;
	auto symbol = match('a', 'z') | match('A', 'Z') | match["_"];
	auto number = match('0', '9');
	auto identifier = symbol & *(symbol | number);
	auto token = identifier >> [](const char* b, const char* e) {
		return std::string("identifier{\"") + std::string(b, e)  + "\"}";
	};
	BOOST_TEST(token("abcd") == std::string(R"(identifier{"abcd"})"));
}

BOOST_AUTO_TEST_CASE(lexer)
{
	const auto& match = utils::match;
	auto symbol = match('a', 'z') | match('A', 'Z') | match["_"];
	auto number = match('0', '9');
	auto identifier = symbol & *(symbol | number);
	/*auto lexer = utils::tokenize(
		utils::tokens(
			identifier >> [](const char* b, const char* e) {
				return std::string("identifier{\"") + std::string(b, e) + "\"}";
			},
			+number >> [](const char* b, const char* e) {
				return std::string("number{" + std::string(b, e) + "}");
			}
		),
		utils::channel::input("")
	);*/
	//std::cout << tokenize(lexer)("abcd") << std::endl;
	//std::cout << tokenize(lexer)("1234") << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
