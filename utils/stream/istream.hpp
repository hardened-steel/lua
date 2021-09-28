#pragma once
#include <utils/buffer.hpp>
#include <utils/channel/ichannel.hpp>

namespace utils::stream {

	template<class T>
	class istream: public utils::channel::ichannel<utils::buffer::owner<const T>>
	{
	public:
		virtual void get(utils::buffer::view<T>&) = 0;
		virtual ~istream() = default;
	public:
	};
}
