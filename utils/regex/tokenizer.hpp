#pragma once
#include <system_error>
#include <utils/regex/regular.hpp>
#include <utils/stream/items.hpp>
#include <utils/channel/input.hpp>
#include <utils/stream/istream.hpp>
#include <utils/stream/ostream.hpp>
#include <utils/tag.hpp>
#include <optional>

namespace utils {

#if 0

	/*template<class Regex, class T>
	class token
	{
		regex<Regex> match;
		token_trait<T> trait;
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
	};*/

	/*class token_t
	{
		template<class T>
		struct trait_t;
	public:
		template<class T>
		class value;
	public:
		template<class Regex, class T>
		class token_m;
	public:
		template<class T, class Regex>
		auto operator()(regex<Regex> match, const tag_t<T>&) const;
	};*/

	struct token_tag {};

	class token_b
	{
	public:
		const token_tag* tag;
	public:
		constexpr token_b(const token_tag* tag = nullptr) noexcept
		: tag(tag)
		{}
		token_b(const token_b&) = default;
		token_b& operator=(const token_b&) = default;
	};

	template<class T, class Regex, class Lexer>
	class token_t: public token_tag
	{
	public:
		using value_t = T;
	public:
		template<class It, class End>
		std::optional<value_t> operator()(It& it, End end) const
		{
			auto begin = it;
			if(match(it, end))
				return lexer.cast<T>(begin, it);
			return std::nullopt;
		}
	public:
		template<class Match>
		constexpr token_t(Lexer& lexer, Match match) noexcept
		: match(std::forward<Match>(match)), lexer(lexer)
		{}
	private:
		regex<Regex> match;
		Lexer& lexer;
	};

	template<class T, class Lexer>
	class token_v: public token_b
	{
		using trait_t = token_t<T, Lexer>;
		using value_t = typename trait_t::value_t;
		value_t value;
	public:
		template<class ...Args>
		constexpr token_v(const trait_ptr_t& trait, Args... &&args)
		: token_b(trait)
		, value(std::forward<Args>(args)...)
		{}
		token_v(const token_v&) = default;
		token_v(token_v&&) = default;
		token_v& operator=(const token_v&) = default;
		token_v& operator=(token_v&&) = default;
	};

	/*template<class In>
	void tokenize(stream::istream<In>& input, stream::ostream<token_v>& out)
	{

	}*/ 

	template<class Regex, class T>
	class token_m
	{
	public:
		template<class Begin, class End>
		bool operator()(Begin& it, End end) const;
	};

	class lexer
	{
	public:
		template<class Regex, class T>
		void add(const token_m<Regex, T>& matcher);
	};





	extern const token_t token;

	/*template<class Regex, class Fn>
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
			return tokens(begin, end);
		};
	}*/

	/*template<class Range, class Fn>
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
	};*/

	/*template<class Regex, class Fn>
	auto operator>>(regex<Regex> match, Fn constructor)
	{
		return token<Regex, Fn>(match, constructor);
	}*/

#endif

}