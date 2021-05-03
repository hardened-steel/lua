#pragma once
#include <system_error>
#include <utils/regex/regular.hpp>
#include <utils/stream/items.hpp>
#include <utils/channel/transform.hpp>
#include <utils/channel/input.hpp>

namespace utils {

	template<class Regex, class Fn>
	class token
	{
		regex<Regex> match;
		Fn constructor;
	public:
		token(regex<Regex> match, Fn constructor) noexcept
		: match(match), constructor(std::move(constructor))
		{}
		template<class Begin, class End>
		bool parse(Begin& it, End end) const
		{
			return match(it, end);
		}
		template<class Begin, class End>
		auto create(Begin beging, End end) const
		{
			return constructor(beging, end);
		}
		template<class Begin, class End>
		decltype(auto) operator()(Begin& it, End end) const
		{
			Begin beging = it;
			if(parse(it, end))
			{
				return create(beging, it);
			}
			throw std::system_error(make_error_code(std::errc::illegal_byte_sequence));
		}
		decltype(auto) operator()(const char* string) const
		{
			auto it = string;
			auto end = string + std::strlen(string);
			return (*this)(it, end);
		}
	};

	template<class Regex, class Fn>
	auto tokens(token<Regex, Fn> token)
	{
		return [token = std::move(token)](auto& it, auto end)
		{
			return token(it, end);
		};
	}

	template<class Regex, class Fn, class ...Tokens>
	auto tokens(token<Regex, Fn> token, Tokens... tokenizers)
	{
		return [token = std::move(token), tokens = tokens(tokenizers...)](auto& begin, auto end)
		{
			auto it = begin;
			if(token.parse(it, end))
				return token.create(begin, it);
			return tokens(it, end);
		};
	}

	template<class Range, class Fn>
	class lexer
	{
		Range range;
		Fn tokenize;
	public:
		template<class IRange>
		lexer(IRange&& range, Fn tokenize)
		: range(std::forward<IRange>(range))
		, tokenize(std::forward<Fn>(tokenize))
		{}
	public:
		/*using value_type = decltype(Fn(channel));
		auto get()
		{
			return function(channel);
		}
		operator bool() const noexcept
		{
			return static_cast<bool>(channel);
		}
		void close()
		{
			channel.close();
		}*/
	};

	template<class Regex, class Fn>
	auto operator>>(regex<Regex> match, Fn constructor)
	{
		return token<Regex, Fn>(match, constructor);
	}

}