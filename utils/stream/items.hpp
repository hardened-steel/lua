#pragma once
#include <memory>
#include <vector>
#include <optional>
#include <utils/channel/ichannel.hpp>
#include <utils/buffer.hpp>

namespace utils {
	template<class IChannel>
	class items
	{
		using Buffer = typename IChannel::value_type;
		struct Record
		{
			Buffer buffer;
			std::shared_ptr<Record> next;
		public:
			Record(Buffer buffer, std::shared_ptr<Record> next) noexcept
			: buffer(std::move(buffer))
			, next(std::move(next))
			{}
		};
		IChannel ichannel;
	public:
		items(IChannel&& ichannel) noexcept
		: ichannel(std::move(ichannel))
		{}
		items(items&&) = default;
		items(const items&) = default;
		items& operator=(const items&) = default;
		items& operator=(items&&) = default;
	public:
		class iterator
		{
			IChannel* ichannel = nullptr;
			std::shared_ptr<Record> record;
			std::size_t index = 0;
		public:
			using iterator_category = std::forward_iterator_tag;
			using difference_type   = std::ptrdiff_t;
			using value_type        = typename Buffer::value_type;
			using pointer           = std::remove_reference_t<value_type>*;
			using reference         = std::remove_reference_t<value_type>&;
		public:
			iterator() = default;
			iterator(IChannel& ichannel, std::shared_ptr<Record> record) noexcept
			: ichannel(&ichannel)
			, record(std::move(record))
			{}
			iterator(const iterator& other) noexcept
			: ichannel(other.ichannel)
			, record(other.record)
			, index(index)
			{}
			iterator& operator=(const iterator& other) noexcept
			{
				if(this != &other) {
					ichannel = other.ichannel;
					record = other.record;
					index = other.index;
				}
				return *this;
			}
			decltype(auto) operator*() const
			{
				return record->buffer[index];
			}
			iterator& operator++()
			{
				if(++index == record->buffer.size()) {
					if(!record->next) {
						if(!ichannel->closed())
							record->next = std::make_shared<Record>(ichannel->get(), nullptr);
						else
							ichannel = nullptr;
					}
					record = record->next;
					index = 0;
				}
				return *this;
			}
			iterator operator++(int)
			{
				++(*this);
				return iiterator(*this);
			}
			friend bool operator==(const iterator& a, const iterator& b) noexcept
			{
				return a.ichannel == b.ichannel && a.index == b.index && a.record == b.record;
			}
			friend bool operator!=(const iterator& a, const iterator& b) noexcept
			{
				return a.ichannel != b.ichannel || a.index != b.index || a.record != b.record;
			}
		};
	public:
		auto begin()
		{
			if(ichannel)
				return iterator(ichannel, std::make_shared<Record>(ichannel.get(), nullptr));
			return iterator();
		}
		auto end() const noexcept
		{
			return iterator();
		}
	};

	template<class Channel>
	items(Channel&& ichannel) -> items<std::remove_cv_t<std::remove_reference_t<Channel>>>;
}