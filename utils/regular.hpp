#pragma once
#include <utility>
#include <cstring>
#include <algorithm>

namespace utils {

	template<class T>
	class regex;

	struct match_sequence_t;

	template<>
	class regex<match_sequence_t>
	{
		const char* value;
		std::size_t size;
	public:
		template<std::size_t Size>
		constexpr regex(const char(&value)[Size]) noexcept
		: value(value), size(Size - 1u)
		{}
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		template<class Begin, class End>
		bool operator()(Begin& it, End vend) const
		{
			Begin vit = it;
			auto mit = value;
			auto mend = value + size;
			for(; vit != vend && mit != mend; ++vit, ++mit)
			{
				if(*vit != *mit)
					return false;
			}
			if(mit == mend)
			{
				it += size;
				return true;
			}
			return false;
		}
		auto operator()(const char* string) const
		{
			auto size = std::strlen(string);
			return (*this)(string, string + size);
		}
	};

	struct match_anyof_t;

	template<>
	class regex<match_anyof_t>
	{
		const char* value;
		std::size_t size;
	public:
		template<std::size_t Size>
		constexpr regex(const char(&value)[Size]) noexcept
		: value(value), size(Size - 1u)
		{}
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		template<class Begin, class End>
		bool operator()(Begin& it, End end) const
		{
			const auto mbegin = value;
			const auto mend = value + size;
			if((it != end) && (std::find(mbegin, mend, *it) != mend))
			{
				it += 1;
				return true;
			}
			return false;
		}
		auto operator()(const char* string) const
		{
			auto size = std::strlen(string);
			return (*this)(string, string + size);
		}
	};

	struct match_range_t;

	template<>
	class regex<match_range_t>
	{
		char from, to;
	public:
		constexpr regex(char from, char to) noexcept
		: from(from), to(to)
		{}
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		template<class Begin, class End>
		bool operator()(Begin& it, End end) const
		{
			if(it != end)
			{
				if(char symbol = *it; symbol >= from && symbol <= to)
				{
					it += 1;
					return true;
				}
			}
			return false;
		}
		auto operator()(const char* string) const
		{
			auto size = std::strlen(string);
			return (*this)(string, string + size);
		}
	};

	struct match_t
	{
		template<std::size_t Size>
		constexpr auto operator()(const char(&value)[Size]) const noexcept
		{
			return regex<match_sequence_t>(value);
		}
		template<std::size_t Size>
		constexpr auto operator[](const char(&value)[Size]) const noexcept
		{
			return regex<match_anyof_t>(value);
		}
		constexpr auto operator()(char from, char to) const noexcept
		{
			if(from < to)
				return regex<match_range_t>(from, to);
			return regex<match_range_t>(to, from);
		}
	} const match;

	template<class MatchA, class MatchB>
	struct match_or_t;

	template<class MatchA, class MatchB>
	class regex<match_or_t<MatchA, MatchB>>
	{
		MatchA match_a;
		MatchB match_b;
	public:
		constexpr regex(const MatchA& match_a, const MatchB& match_b) noexcept
		: match_a(match_a), match_b(match_b)
		{}
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		template<class Begin, class End>
		bool operator()(Begin& it, End end) const
		{

			return match_a(it, end) || match_b(it, end);
		}
		auto operator()(const char* string) const
		{
			auto size = std::strlen(string);
			return (*this)(string, string + size);
		}
	};

	template<class RegexA, class RegexB>
	constexpr auto operator|(regex<RegexA> a, regex<RegexB> b) noexcept
	{
		return regex<match_or_t<regex<RegexA>, regex<RegexB>>>(std::move(a), std::move(b));
	}
	template<std::size_t Size>
	constexpr regex<match_anyof_t> operator|(const regex<match_anyof_t>& a, const char(&b)[Size]) noexcept
	{
		return a | match[b];
	}
	template<std::size_t Size>
	constexpr regex<match_anyof_t> operator|(const char(&a)[Size], const regex<match_anyof_t>& b) noexcept
	{
		return match[a] | b;
	}

	template<class MatchA, class MatchB>
	struct match_and_t;

	template<class MatchA, class MatchB>
	class regex<match_and_t<MatchA, MatchB>>
	{
		MatchA match_a;
		MatchB match_b;
	public:
		constexpr regex(const MatchA& match_a, const MatchB& match_b) noexcept
		: match_a(match_a), match_b(match_b)
		{}
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		template<class Begin, class End>
		bool operator()(Begin& it, End end) const
		{
			return match_a(it, end) && match_b(it, end);
		}
		auto operator()(const char* string) const
		{
			auto size = std::strlen(string);
			return (*this)(string, string + size);
		}
	};

	template<class RegexA, class RegexB>
	constexpr auto operator&(regex<RegexA> a, regex<RegexB> b) noexcept
	{
		return regex<match_and_t<regex<RegexA>, regex<RegexB>>>(std::move(a), std::move(b));
	}
	template<std::size_t Size>
	constexpr regex<match_sequence_t> operator&(const regex<match_sequence_t>& a, const char(&b)[Size]) noexcept
	{
		return a & match(b);
	}
	template<std::size_t Size>
	constexpr regex<match_sequence_t> operator&(const char(&a)[Size], const regex<match_sequence_t>& b) noexcept
	{
		return match(a) & b;
	}

	template<class Match>
	struct match_zero_plus;

	template<class Match>
	class regex<match_zero_plus<Match>>
	{
		Match match;
	public:
		constexpr regex(const Match& match) noexcept
		: match(match)
		{}
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		template<class Begin, class End>
		bool operator()(Begin& it, End end) const
		{
			while(match(it, end));
			return true;
		}
		auto operator()(const char* string) const
		{
			auto size = std::strlen(string);
			return (*this)(string, string + size);
		}
	};

	template<class Match>
	constexpr auto operator *(regex<Match> match) noexcept
	{
		return regex<match_zero_plus<regex<Match>>>(std::move(match));
	}

	template<class Match>
	struct match_optional;

	template<class Match>
	class regex<match_optional<Match>>
	{
		Match match;
	public:
		constexpr regex(const Match& match) noexcept
		: match(match)
		{}
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		template<class Begin, class End>
		bool operator()(Begin& it, End end) const
		{
			return match(it, end) || true;
		}
		auto operator()(const char* string) const
		{
			auto size = std::strlen(string);
			return (*this)(string, size);
		}
	};

	template<class Match>
	constexpr auto operator !(regex<Match> match) noexcept
	{
		return regex<match_optional<regex<Match>>>(std::move(match));
	}

	template<class Match>
	struct match_one_plus;

	template<class Match>
	class regex<match_one_plus<Match>>
	{
		Match match;
	public:
		constexpr regex(const Match& match) noexcept
		: match(match)
		{}
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		template<class Begin, class End>
		bool operator()(Begin& it, End end) const
		{
			if(match(it, end))
			{
				while(match(it, end));
				return true;
			}
			return false;
		}
		auto operator()(const char* string) const
		{
			auto size = std::strlen(string);
			return (*this)(string, size);
		}
	};

	template<class Match>
	constexpr auto operator +(regex<Match> match) noexcept
	{
		return regex<match_one_plus<regex<Match>>>(std::move(match));
	}

}