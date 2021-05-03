#include <boost/iostreams/device/mapped_file.hpp>
#include <system_error>
#include "fstream.hpp"

namespace bio = boost::iostreams;

namespace {

	struct ROFileMapping
	{
		bio::mapped_file_source source;
	public:
		ROFileMapping(const std::filesystem::path& path)
		: source(path.string())
		{}
	public:
		const std::byte* data() const noexcept
		{
			return reinterpret_cast<const std::byte*>(source.data());
		}
		std::size_t size() const noexcept
		{
			return source.size();
		}
	};
}

namespace utils::stream {
	ifstream::ifstream(const std::filesystem::path& path)
	: mmap(ROFileMapping(path))
	{
	}

	void ifstream::get(utils::buffer::view<std::byte>& view)
	{
		auto chunk = get(view.size());
		view = view.first(chunk.size());
		std::copy(chunk.begin(), chunk.end(), view.data());
	}

	utils::buffer::owner<const std::byte> ifstream::get(std::size_t count)
	{
		if(count)
			return mmap.first(std::min(count, mmap.size()));
		return std::move(mmap);
	}

	void ifstream::close()
	{
	}

	bool ifstream::closed() const noexcept
	{
		return mmap.size();
	}
}