#define BOOST_TEST_MODULE utils_io
#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include <utils/channel/channel.hpp>
#include <utils/channel/input.hpp>
#include <utils/channel/output.hpp>
#include <utils/channel/transform.hpp>

#include <cstdlib>
#include <memory>
#include <cxxabi.h>

namespace {
	std::string demangle(const char* name) {

	    int status = -4; // some arbitrary value to eliminate the compiler warning
	    return abi::__cxa_demangle(name, NULL, NULL, &status);
	}
}

class foo
{
public:
	std::string name;
public:
	foo(std::string name) noexcept
	: name(std::move(name))
	{}
	foo(const foo& other)
	: name(other.name)
	{
		std::cout << "foo::copy construcor: " << name << std::endl;
	}
	foo(foo&& other) noexcept
	: name(std::move(other.name))
	{
		std::cout << "foo::move construcor: " << name << std::endl;
	}
	foo& operator=(foo& other) noexcept
	{
		if(this != &other) {
			std::cout << "foo::move assign: " << name << " = " << other.name << std::endl;
			name = std::move(other.name);
		}
		return *this;
	}
	foo& operator=(const foo& other)
	{
		if(this != &other) {
			std::cout << "foo::copy assign: " << name << " = " << other.name << std::endl;
			name = other.name;
		}
		return *this;
	}
	~foo()
	{
		std::cout << "foo::destroy: " << name << std::endl;
	}
	friend std::ostream& operator<<(std::ostream& stream, const foo& value)
	{
		return stream << "{foo: " << value.name << "}";
	}
};

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


BOOST_AUTO_TEST_CASE(channel_base)
{
	std::vector<foo> result;
	result.reserve(2);
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
