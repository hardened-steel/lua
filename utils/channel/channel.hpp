#pragma once
#include <utility>
#include <utils/channel/ichannel.hpp>
#include <utils/channel/ochannel.hpp>

namespace utils::channel {
	template<class IChannel, class OChannel>
	void operator >> (IChannel ichannel, OChannel ochannel)
	{
		for(decltype(auto) value: irange(ichannel)) {
			if(ochannel) {
				ochannel.put(std::move(value));
			} else {
				ochannel.close();
				break;
			}
		}
	}
}