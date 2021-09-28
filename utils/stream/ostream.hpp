#pragma once
#include <utils/buffer.hpp>
#include <utils/channel/ochannel.hpp>

namespace utils::stream {

	template<class T>
	class ostream: public utils::channel::ochannel<utils::buffer::owner<T>>
	{
	public:
		virtual void put(utils::buffer::view<const T>&) = 0;
		virtual ~ostream() = default;
	public:
	};
}
