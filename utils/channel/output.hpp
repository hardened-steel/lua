#pragma once
#include <utils/channel/ochannel.hpp>

namespace utils::channel {

	template<class T, class Iterator = T*>
	class output
	{
		Iterator begin_;
		Iterator end_;
	public:
		output(Iterator begin, Iterator end) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
		: begin_(std::move(begin))
		, end_(std::move(end))
		{}
		template<class Container>
		output(Container container) noexcept(noexcept(std::begin(container)) && noexcept(std::end(container)) && std::is_nothrow_move_constructible_v<Iterator>)
		: begin_(std::begin(container))
		, end_(std::end(container))
		{}
		template<std::size_t N>
		output(T(&array)[N]) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
		: begin_(array)
		, end_(array + N)
		{}
	public:
		void put(T value)
		{
			*begin_ = std::move(value);
			++begin_;
		}
		operator bool() const noexcept
		{
			return begin_ != end_;
		}
		void close()
		{
			begin_ = end_;
		}
	};

	template<class Container> output(Container&) -> output<typename Container::value_type, typename Container::iterator>;
	template<class T, std::size_t N> output(T(&)[N]) -> output<T, T*>;

	template<class T, class Container>
	class output<T, std::back_insert_iterator<Container>>: public ochannel<T>
	{
		using Iterator = std::back_insert_iterator<Container>;
		Iterator it;
		bool closed = false;
	public:
		output(Iterator it) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
		: it(std::move(it))
		{}
	public:
		void put(T value)
		{
			*it = std::move(value);
			++it;
		}
		operator bool() const noexcept
		{
			return !closed;
		}
		void close()
		{
			closed = true;
		}
	};

	template<class Container> output(std::back_insert_iterator<Container>) -> output<typename Container::value_type, std::back_insert_iterator<Container>>;
}