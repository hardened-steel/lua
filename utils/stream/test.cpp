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

namespace {

	auto& operator << (std::ostream& stream, std::byte byte)
	{
		const char symbols[] = "0123456789ABCDEF";
		auto value = std::to_integer<unsigned char>(byte);
		return stream << symbols[(value & 0xF0) >> 4u] << symbols[(value & 0x0F) >> 0u];
	}
}

BOOST_AUTO_TEST_SUITE(stream)

BOOST_AUTO_TEST_CASE(file)
{
	using utils::operator""_bytes;
	const fs::path path = "fstream-test.data";
	TouchFile(path, "hello from file");
	utils::stream::ifstream file(path);
	auto value = file.get(1024);
	BOOST_TEST(value == "hello from file"_bytes);
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
