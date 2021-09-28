#pragma once
#include <utility>
#include <memory>

namespace utils {

	struct buffer
	{
		template<class T> class view;
		template<class T> class owner;
		template<class T>
		struct Resource
		{
			virtual T* begin() const noexcept = 0;
			virtual T* end() const noexcept = 0;
		};
	};

	template<class T>
	class buffer::view
	{
		T* begin_ = nullptr;
		T* end_ = nullptr;
	public:
		using value_type = T;
	public:
		template<class It>
		view(It b, It e) noexcept
		: begin_(&(*b)), end_(&(*e))
		{}
		view(T *array, std::size_t size) noexcept
		: begin_(array), end_(array + size)
		{}
		template<std::size_t Size>
		view(T(&value)[Size]) noexcept
		: begin_(value), end_(value + Size)
		{}
		view() = default;
		view(const view&) = default;
		view& operator=(const view&) = default;
	public:
		auto begin() const noexcept
		{
			return begin_;
		}
		auto end() const noexcept
		{
			return end_;
		}
		T* data() const noexcept
		{
			return begin_;
		}
		std::size_t size() const noexcept
		{
			return end_ - begin_;
		}
		auto& front() const noexcept
		{
			return *begin();
		}
		auto& back() const noexcept
		{
			return *(data() + size() - 1u);
		}
		bool empty() const noexcept
		{
			return begin_ == end_;
		}
	public:
		auto& operator[](std::size_t index) const noexcept
		{
			return data()[index];
		}
		view first(std::size_t count) noexcept
		{
			count = std::min(count, size());
			view result(data(), data() + count);
			begin_ += count;
			return result;
		}
		view last(std::size_t count) noexcept
		{
			count = std::min(count, size());
			view result(end() - count, end());
			end_ -= count;
			return result;
		}
		operator view<const T>() const noexcept
		{
			return view<const T>(*this);
		}
		friend bool operator==(const view& a, const view& b) noexcept
		{
			return std::equal(a.begin(), a.end(), b.begin(), b.end());
		}
		friend bool operator!=(const view& a, const view& b) noexcept
		{
			return !std::equal(a.begin(), a.end(), b.begin(), b.end());
		}
	};

	inline auto operator""_bytes(const char* str, std::size_t size) noexcept
	{
		return  buffer::view<const std::byte>(reinterpret_cast<const std::byte*>(str), size);
	}
	inline auto operator""_view(const char* str, std::size_t size) noexcept
	{
		return  buffer::view<const char>(str, size);
	}

	template<class T>
	class buffer::owner: public buffer::view<T>
	{
		using base = buffer::view<T>;
	private:
		template<class Container>
		struct ContainerResource: Resource<T>
		{
			Container container;
			T* begin() const noexcept override
			{
				return container.data();
			}
			T* end() const noexcept override
			{
				return container.data() + container.size();
			}
			ContainerResource(Container&& container) noexcept
			: container(std::move(container))
			{}
		};
		std::shared_ptr<Resource<T>> resource = nullptr;
	public:
		template<class R>
		owner(std::shared_ptr<R> resource) noexcept
		: base(resource->begin(), resource->end())
		, resource(std::move(resource))
		{}
		template<class R>
		owner(std::shared_ptr<R> resource, buffer::view<T> view) noexcept
		: base(view)
		, resource(std::move(resource))
		{}
		template<class It>
		owner(It&& b, It&& e) noexcept
		: base(std::forward<It>(b), std::forward<It>(e))
		{}
		template<class Container>
		owner(Container c)
		: owner(std::make_shared<ContainerResource<Container>>(std::forward<Container>(c)))
		{}
		template<std::size_t Size>
		owner(T(&value)[Size]) noexcept
		: base(value)
		{}
		owner() = default;
		owner(buffer::view<T>) = delete;
		owner(const owner&) = delete;
		owner(owner&&) = default;
		owner& operator=(const owner&) = delete;
		owner& operator=(owner&&) = default;
	public:
		owner first(std::size_t count) noexcept
		{
			return owner(resource, base::first(count));
		}
		owner last(std::size_t count) noexcept
		{
			return owner(resource, base::last(count));
		}
		owner share() noexcept
		{
			return owner(resource);
		}
	public:
		operator owner<const T>() const noexcept
		{
			return view<const T>(*this);
		}
	};

}