#pragma once
#include <utility>
#include <iterator>

namespace utils::channel {

	template<class T>
	class ichannel
	{
	public:
		virtual ~ichannel() = default;
		virtual T get() = 0;
		virtual bool closed() const noexcept = 0;
		virtual void close() = 0;
	public:
		using value_type = T;
	public:
		operator bool() const noexcept
		{
			return closed();
		}
	};

	template<class Channel>
	class irange_t
	{
	public:
		using value_type = std::remove_cv_t<std::remove_reference_t<typename Channel::value_type>>;
		Channel channel;
		value_type value;
	public:
		class iterator
		{
			irange_t *range;
		public:
			using iterator_category = std::input_iterator_tag;
			using difference_type   = std::ptrdiff_t;
			using value_type        = typename Channel::value_type;
			using pointer           = std::remove_reference_t<value_type>*;
			using reference         = std::remove_reference_t<value_type>&;
		public:
			iterator(irange_t *range) noexcept
			: range(range)
			{}
			auto& operator*() const noexcept
			{
				return range->value;
			}
			iterator& operator++()
			{
				if(!range->channel.closed()) {
					range = nullptr;
					return *this;
				}
				range->value = range->channel.get();
				return *this;
			}
			friend bool operator==(const iterator& a, const iterator& b) noexcept
			{
				return a.range == b.range;
			}
			friend bool operator!=(const iterator& a, const iterator& b) noexcept
			{
				return a.range != b.range;
			}
		};
	public:
		template<class T>
		irange_t(Channel channel, T&& value = T{})
		: channel(std::forward<Channel>(channel)), value(std::forward<T>(value))
		{}
		irange_t(Channel channel)
		: channel(std::forward<Channel>(channel))
		{}
		auto begin()
		{
			return ++iterator(this);
		}
		auto end()
		{
			return iterator(nullptr);
		}
	};

	template<class Channel, class T>
	auto irange(Channel&& channel, T&& value)
	{
		return irange_t<std::remove_reference_t<Channel>>(std::forward<Channel>(channel), std::forward<T>(value));
	}
	template<class Channel>
	auto irange(Channel&& channel)
	{
		return irange_t<std::remove_reference_t<Channel>>(std::forward<Channel>(channel));
	}
	
}