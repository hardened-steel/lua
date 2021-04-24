#include <gtest/gtest.h>
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

TEST(stream, file)
{
	using utils::operator""_bytes;
	const fs::path path = "fstream-test.data";
	TouchFile(path, "hello from file");
	utils::stream::ifstream file(path);
	auto value = file.get(1024);
	EXPECT_TRUE(value == "hello from file"_bytes);
}

TEST(stream_DISABLED, items)
{
	using utils::operator""_bytes;
	const fs::path path = "fstream-test.data";
	TouchFile(path, "hello from file");
	utils::items fchannel = utils::stream::ifstream(path);
	const auto str = "hello from file"_bytes;
	EXPECT_TRUE(std::equal(fchannel.begin(), fchannel.end(), str.begin(), str.end()));
}