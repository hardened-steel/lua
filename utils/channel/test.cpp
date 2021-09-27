#include <catch2/catch_test_macros.hpp>

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

TEST_CASE("test utils channel", "[channel]")
{
	const char hello[] = "hello world";
	utils::channel::input ichannel{hello};
	REQUIRE(ichannel.get() == 'h');
	REQUIRE(ichannel.get() == 'e');
	REQUIRE(ichannel.get() == 'l');
	REQUIRE(ichannel.get() == 'l');
	REQUIRE((utils::channel::input{hello} >> vectorize) == (std::vector<char>(hello, hello + sizeof(hello))));
}

TEST_CASE("test utils channel", "[transform]")
{
	auto pow2 = [](auto& value)
	{
		return value * value;
	};
	REQUIRE((utils::channel::input({1, 2, 3}) >> utils::channel::transform(pow2) >> vectorize) == (utils::channel::input({1, 4, 9}) >> vectorize));
}
