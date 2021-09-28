#include <catch2/catch_test_macros.hpp>

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

using items_ifstream_iterator = typename utils::items<utils::stream::ifstream>::iterator;

TEST_CASE("test file stream", "[utils], [stream]")
{
	using utils::operator""_bytes;
	const fs::path path = "fstream-test.data";
	TouchFile(path, "hello from file");
	utils::stream::ifstream file(path);
	auto value = file.get();
	REQUIRE(value == "hello from file"_bytes);
}

TEST_CASE("test stream items with copy", "[utils], [stream]")
{
	using utils::operator""_bytes;
	const fs::path path = "fstream-test.data";
	TouchFile(path, "hello from file");
	utils::items fchannel = utils::stream::ifstream(path);
	auto tmp = fchannel.begin();
	auto it = tmp;
	REQUIRE(*it == std::byte('h')); ++it;
	REQUIRE(*it == std::byte('e')); ++it;
	REQUIRE(*it == std::byte('l')); ++it;
	REQUIRE(*it == std::byte('l')); ++it;
	REQUIRE(*it == std::byte('o')); ++it;

	REQUIRE(*it == std::byte(' ')); ++it;

	REQUIRE(*it == std::byte('f')); ++it;
	REQUIRE(*it == std::byte('r')); ++it;
	REQUIRE(*it == std::byte('o')); ++it;
	REQUIRE(*it == std::byte('m')); ++it;

	REQUIRE(*it == std::byte(' ')); ++it;

	REQUIRE(*it == std::byte('f')); ++it;
	REQUIRE(*it == std::byte('i')); ++it;
	REQUIRE(*it == std::byte('l')); ++it;
	REQUIRE(*it == std::byte('e')); ++it;
	INFO("'it' iterator: " << &(*it));
	//REQUIRE(it == fchannel.end());
}

TEST_CASE("test file stream with items", "[utils], [stream]")
{
	using utils::operator""_bytes;
	const fs::path path = "fstream-test.data";
	TouchFile(path, "hello from file");
	utils::items fchannel = utils::stream::ifstream(path);
	const auto str = "hello from file"_bytes;
	//REQUIRE(std::equal(fchannel.begin(), fchannel.end(), str.begin(), str.end()));
}
