#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <unistd.h>

#include <system_error>
#include "fstream.hpp"

namespace {

	struct FileDescriptor
	{
		const int fd;
	public:
		FileDescriptor(int fd) noexcept
		: fd(fd)
		{}
		~FileDescriptor() noexcept
		{
			close(fd);
		}
		operator int() const noexcept
		{
			return fd;
		}
	};

	struct ROFileMapping
	{
		std::size_t length;
		std::byte* start = nullptr;
	private:
		static FileDescriptor OpenFile(const std::filesystem::path& path)
		{
			int fd = open(path.string().c_str(), O_RDONLY);
			if(fd < 0)
				throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)));
			return FileDescriptor(fd);
		}
		static std::size_t GetFileLength(const FileDescriptor& fd)
		{
			struct stat fileInfo = {0};
			if(fstat(fd, &fileInfo) == -1)
				throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)));
			return fileInfo.st_size;
		}
		static std::byte* MMapFile(const FileDescriptor& fd, std::size_t length)
		{
			auto ptr = mmap(nullptr, length, PROT_READ, MAP_SHARED, fd, 0);
			if(ptr == MAP_FAILED)
				throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)));
			return static_cast<std::byte*>(ptr);
		}
		ROFileMapping(FileDescriptor fd)
		: length(GetFileLength(fd))
		, start(MMapFile(fd, length))
		{
			madvise(start, length, MADV_SEQUENTIAL);
		}
	public:
		ROFileMapping(const std::filesystem::path& path)
		: ROFileMapping(OpenFile(path))
		{}
		~ROFileMapping() noexcept
		{
			if(start && length)
				munmap(start, length);
		}
		ROFileMapping(ROFileMapping&& other) noexcept
		: length(other.length)
		, start(other.start)
		{
			other.start = nullptr;
		}
		ROFileMapping& operator=(ROFileMapping&& other) noexcept
		{
			if(this != &other)
			{
				length = other.length;
				start = other.start;
				other.start = nullptr;
			}
			return *this;
		}
	public:
		const std::byte* data() const noexcept
		{
			return start;
		}
		std::size_t size() const noexcept
		{
			return length;
		}
	};
}

namespace utils::stream {
	ifstream::ifstream(const std::filesystem::path& path)
	: mmap(ROFileMapping(path))
	{
	}

	void ifstream::get(utils::buffer::view<std::byte> view)
	{
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