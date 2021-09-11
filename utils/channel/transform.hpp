#pragma once
#include <functional>
#include <utils/channel/ichannel.hpp>

namespace utils::channel {

	struct Transformer
	{
		template<class Fn>
		struct Function
		{
			Fn function;
		};

		template<class Fn>
		auto operator()(Fn&& function) const
		{
			return Function<Fn>{std::forward<Fn>(function)};
		}
	};

	template<class IChannel, class Fn>
	class itransformator
	{
		IChannel channel;
		Transformer::Function<Fn> transform;
	public:
		template<class Channel>
		itransformator(Channel&& channel, Transformer::Function<Fn>&& transform)
		: channel(std::forward<Channel>(channel))
		, transform(std::move(transform))
		{}
	public:
		using value_type = decltype(transform.function(channel.get()));
		decltype(auto) get()
		{
			return transform.function(channel.get());
		}
		bool closed() const noexcept
		{
			return channel.closed();
		}
		void close()
		{
			channel.close();
		}
	};

	template<class IChannel, class Fn>
	auto operator >> (IChannel&& channel, Transformer::Function<Fn>&& transform)
	{
		using Channel = std::remove_cv_t<std::remove_reference_t<IChannel>>;
		return itransformator<Channel, Fn>(std::forward<IChannel>(channel), std::move(transform));
	}

	template<class OChannel, class Fn>
	class otransformator
	{
		OChannel channel;
		Transformer::Function<Fn> transform;
	public:
		template<class Channel>
		otransformator(Channel&& channel, Transformer::Function<Fn>&& transform)
		: channel(std::forward<Channel>(channel))
		, transform(std::move(transform))
		{}
	public:
		using value_type = decltype(transform.function(channel.get()));
		template<class T>
		void put(T&& value)
		{
			return channel.put(transform.function(std::forward<T>(value)));
		}
		bool closed() const noexcept
		{
			return channel.closed();
		}
		void close()
		{
			channel.close();
		}
	};

	inline const Transformer transform;

	template<class OChannel, class Fn>
	auto operator << (OChannel&& channel, Transformer::Function<Fn>&& transform)
	{
		using Channel = std::remove_cv_t<std::remove_reference_t<OChannel>>;
		return otransformator<Channel, Fn>(std::forward<OChannel>(channel), std::move(transform));
	}

}