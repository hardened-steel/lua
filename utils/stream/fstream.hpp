#pragma once
#include <cstddef>
#include <cstdio>
#include <utils/stream/istream.hpp>
#include <filesystem>

namespace utils::stream {

	class ifstream: public istream<std::byte>
	{
		buffer::owner<const std::byte> mmap;
	public:
		ifstream(const std::filesystem::path& path);
	public:
		using value_type = utils::buffer::owner<const std::byte>;
		void get(utils::buffer::view<std::byte>) override;
		utils::buffer::owner<const std::byte> get(std::size_t count) override;
		utils::buffer::owner<const std::byte> get() override
		{
			return get(0);
		}
		void close() override;
		bool closed() const noexcept override;
	};

}