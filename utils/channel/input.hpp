#pragma once
#include <utils/channel/ichannel.hpp>

namespace utils::channel {

	template<class T, class Iterator = T*>
	class input
	{
		Iterator begin_;
		Iterator end_;
	public:
		input(const input&) = delete;
		input(input&&) = default;
		input(Iterator begin, Iterator end) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
		: begin_(std::move(begin))
		, end_(std::move(end))
		{}
		template<class Container>
		input(Container&& container) noexcept(noexcept(std::begin(container)) && noexcept(std::end(container)))
		: begin_(std::begin(container))
		, end_(std::end(container))
		{}
		template<std::size_t N>
		input(std::remove_reference_t<T>(&&array)[N]) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
		: begin_(array)
		, end_(array + N)
		{}
		template<std::size_t N>
		input(const std::remove_reference_t<T>(&array)[N]) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
		: begin_(array)
		, end_(array + N)
		{}
	public:
		using value_type = T;
		T get()
		{
			return *begin_++;
		}
		bool closed() const noexcept
		{
			return begin_ != end_;
		}
		void close()
		{
			begin_ = end_;
		}
	};

	template<class T> input(std::initializer_list<T> init) -> input<const T&, const T*>;
	template<class T, std::size_t N> input(T(&&)[N]) -> input<T&, T*>;
	template<class T, std::size_t N> input(const T(&)[N]) -> input<const T&, const T*>;

}