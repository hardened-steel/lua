#define BOOST_TEST_MODULE utils channel unit test
#include <boost/test/included/unit_test.hpp>

#include <utils/channel/channel.hpp>
#include <utils/channel/input.hpp>
#include <utils/channel/output.hpp>
#include <utils/channel/transform.hpp>

namespace {

	struct vectorizer
	{
	} const vectorize;

	template<class IChannel>
	auto operator >> (IChannel&& channel, const vectorizer&)
	{
		std::vector<std::remove_const_t<std::remove_reference_t<typename IChannel::value_type>>> result;
		for(auto& element: utils::channel::irange(std::forward<IChannel>(channel)))
			result.emplace_back(std::forward<typename IChannel::value_type>(element));
		return result;
	}
}


BOOST_AUTO_TEST_SUITE(channel)

BOOST_AUTO_TEST_CASE(base)
{
	const char hello[] = "hello world";
	utils::channel::input ichannel{hello};
	BOOST_TEST(ichannel.get() == 'h');
	BOOST_TEST(ichannel.get() == 'e');
	BOOST_TEST(ichannel.get() == 'l');
	BOOST_TEST(ichannel.get() == 'l');
	BOOST_TEST((utils::channel::input{hello} >> vectorize) == (std::vector<char>(hello, hello + sizeof(hello))));
}

BOOST_AUTO_TEST_CASE(transform)
{
	auto pow2 = [](auto& value)
	{
		return value * value;
	};
	BOOST_TEST((utils::channel::input({1, 2, 3}) >> utils::channel::transform(pow2) >> vectorize) == (utils::channel::input({1, 4, 9}) >> vectorize));
}

BOOST_AUTO_TEST_SUITE_END()
