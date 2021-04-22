#pragma once
#include <string>
#include <string_view>

namespace lua {
	class token
	{
	public:
		template<class T> class tag {};
		inline static const tag<std::string> string;
		inline static const tag<int> integer;
		constexpr const auto& operator[](std::string_view value) const
		{
			return string;
		}
	public:
		class value
		{
			
		};
	};
	inline const token token;

	class lexer
	{
	public:
		class iterator
		{
			
		};
		template<class T>
		auto get(const token::tag<T>& tag)
		{
			return T{};
		}
	};

	namespace details::token {
		class base
		{

		};

		template<class T>
		class typed: public base
		{
			
		};
	}
}