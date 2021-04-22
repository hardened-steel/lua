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

	inline const Transformer transform;

	template<class IChannel, class Fn>
	auto operator >> (IChannel&& channel, Transformer::Function<Fn>&& transform)
	{
		using Channel = std::remove_cv_t<std::remove_reference_t<IChannel>>;
		return itransformator<Channel, Fn>(std::forward<IChannel>(channel), std::move(transform));
	}

}