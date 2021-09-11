#define BOOST_TEST_MODULE utils stream unit test
#include <boost/test/included/unit_test.hpp>

#include <filesystem>
#include <fstream>
#include <utils/stream/fstream.hpp>
#include <utils/stream/items.hpp>

namespace fs = std::filesystem;

void TouchFile(const fs::path& path, const std::string& value)
{
	if(!fs::exists(path))
		std::ofstream(path) << value;
}

namespace std {

	std::ostream& boost_test_print_type(std::ostream& stream, const std::byte& byte)
	{
		const char symbols[] = "0123456789ABCDEF";
		auto value = std::to_integer<unsigned char>(byte);
		return stream << symbols[(value & 0xF0) >> 4u] << symbols[(value & 0x0F) >> 0u];
	}
}

using items_ifstream_iterator = typename utils::items<utils::stream::ifstream>::iterator;
BOOST_TEST_DONT_PRINT_LOG_VALUE(items_ifstream_iterator)

BOOST_AUTO_TEST_SUITE(stream)

BOOST_AUTO_TEST_CASE(file)
{
	using utils::operator""_bytes;
	const fs::path path = "fstream-test.data";
	TouchFile(path, "hello from file");
	utils::stream::ifstream file(path);
	auto value = file.get();
	BOOST_TEST(value == "hello from file"_bytes);
}

BOOST_AUTO_TEST_CASE(items_with_copy)
{
	using utils::operator""_bytes;
	const fs::path path = "fstream-test.data";
	TouchFile(path, "hello from file");
	utils::items fchannel = utils::stream::ifstream(path);
	auto tmp = fchannel.begin();
	auto it = tmp;
	BOOST_TEST(*it == std::byte('h')); ++it;
	BOOST_TEST(*it == std::byte('e')); ++it;
	BOOST_TEST(*it == std::byte('l')); ++it;
	BOOST_TEST(*it == std::byte('l')); ++it;
	BOOST_TEST(*it == std::byte('o')); ++it;

	BOOST_TEST(*it == std::byte(' ')); ++it;

	BOOST_TEST(*it == std::byte('f')); ++it;
	BOOST_TEST(*it == std::byte('r')); ++it;
	BOOST_TEST(*it == std::byte('o')); ++it;
	BOOST_TEST(*it == std::byte('m')); ++it;

	BOOST_TEST(*it == std::byte(' ')); ++it;

	BOOST_TEST(*it == std::byte('f')); ++it;
	BOOST_TEST(*it == std::byte('i')); ++it;
	BOOST_TEST(*it == std::byte('l')); ++it;
	BOOST_TEST(*it == std::byte('e')); ++it;
	BOOST_CHECK_EQUAL(it, fchannel.end());
}

BOOST_AUTO_TEST_CASE(items)
{
	using utils::operator""_bytes;
	const fs::path path = "fstream-test.data";
	TouchFile(path, "hello from file");
	utils::items fchannel = utils::stream::ifstream(path);
	const auto str = "hello from file"_bytes;
	BOOST_TEST(std::equal(fchannel.begin(), fchannel.end(), str.begin(), str.end()));
}

BOOST_AUTO_TEST_SUITE_END()
