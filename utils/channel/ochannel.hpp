#pragma once
#include <utility>
#include <type_traits>

namespace utils::channel {

	template<class T>
	class ochannel
	{
	public:
		virtual ~ochannel() = default;
		virtual void put(T) = 0;
		virtual operator bool() const noexcept = 0;
		virtual void close() = 0;
	public:
		class iterator
		{
			ochannel *channel;
		public:
			iterator(ochannel *channel) noexcept
			: channel(channel)
			{}

			const auto& operator*() const noexcept
			{
				return *this;
			}
			iterator& operator++() noexcept
			{
				if(*channel) 
					return *this;
				channel = nullptr;
				return *this;
			}
			template<class U>
			void operator=(U&& value) const
			{
				channel->put(std::forward<U>(value));
			}
			friend bool operator==(const iterator& a, const iterator& b) noexcept
			{
				return a.channel == b.channel;
			}
        	friend bool operator!=(const iterator& a, const iterator& b) noexcept
			{
				return a.channel != b.channel;
			}
		};

		auto begin()
		{
			return iterator(this);
		}
		auto end()
		{
			return iterator(nullptr);
		}
	};

}