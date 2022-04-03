#pragma once
#include <utility>
#include <cstdint>
#include <string>
#include <optional>
#include <variant>
#include <algorithm>
#include <utils/tag.hpp>
#include <utils/types/tuple.hpp>

namespace utils {

	struct from_params {};

	template<class T>
	class regex;

	template<class T>
	struct match_result
	{
		using type = std::vector<T>;
	};

	template<>
	struct match_result<char>
	{
		using type = std::string;
	};

	template<class T>
	using match_result_t = typename match_result<T>::type;

	template<class T>
	struct match_any {};

	struct match_any_t
	{
		template<class Begin, class End>
		constexpr auto operator()(Begin& it, End end) const
		{
			return regex<match_any<typename std::iterator_traits<Begin>::value_type>>{}(it, end);
		}
		template<class T>
		constexpr auto operator()(T value) const
		{
			match_result_t<T> result;
			result.emplace_back(std::move(value));
			return result;
		}
		template<class T, class Regex>
		constexpr auto operator()(const T&, const regex<Regex>&) const
		{
			return match_result_t<T>();
		}
		template<class T>
		constexpr match_result_t<T> operator()(const T& value, const match_any_t& subrule) const
		{
			if(subrule == *this)
				return subrule(value);
			return {};
		}
		friend constexpr bool operator==(const match_any_t&, const match_any_t&) noexcept
		{
			return true;
		}
		template<class T>
		friend constexpr bool operator==(const match_any_t&, const regex<match_any<T>>&) noexcept
		{
			return true;
		}
		template<class T>
		friend constexpr bool operator==(const regex<match_any<T>>&, const match_any_t&) noexcept
		{
			return true;
		}
	};

	template<class T>
	class regex<match_any<T>>
	{
	public:
		constexpr regex() = default;
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		using input_type = T;
		using value_type = T;
		using result_t = std::optional<T>;
	public:
		template<class Begin, class End>
		constexpr result_t operator()(Begin& it, End end) const
		{
			if(it != end)
			{
				return *it++;
			}
			return std::nullopt;
		}
		constexpr auto operator()(value_type value) const
		{
			match_result_t<T> result;
			result.emplace_back(std::move(value));
			return result;
		}
		template<class Regex>
		constexpr auto operator()(const value_type&, const regex<Regex>&) const
		{
			return match_result_t<T>();
		}
		constexpr match_result_t<T> operator()(const value_type& value, const regex& subrule) const
		{
			if(subrule == *this)
				return subrule(value);
			return {};
		}
		friend constexpr bool operator==(const regex&, const regex&) noexcept
		{
			return true;
		}
	};

	template<class T, std::size_t Size>
	struct match_sequence_t {};

	template<class T, std::size_t Size>
	class regex<match_sequence_t<T, Size>>
	{
		std::array<T, Size> values;
	public:
		template<std::size_t... I>
		constexpr regex(const T(&array)[Size + 1], std::index_sequence<I...>) noexcept
		: values{array[I]...}
		{}
		constexpr regex(const T(&array)[Size + 1]) noexcept
		: regex(array, std::make_index_sequence<Size>())
		{}
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		using input_type = T;
		struct value_type {};
		struct result_t
		{
			bool value = false;
			constexpr operator bool() const noexcept
			{
				return value;
			}
			constexpr value_type operator*() const noexcept
			{
				return value_type{};
			}
			friend constexpr bool operator==(const result_t& a, const result_t& b) noexcept
			{
				return a.value == b.value;
			}
		};
	public:
		template<class Begin, class End>
		constexpr result_t operator()(Begin& it, End vend) const
		{
			Begin vit = it;
			auto mit = values.begin();
			auto mend = values.end();
			for(; vit != vend && mit != mend; ++vit, ++mit)
			{
				if(*vit != *mit)
					return result_t{false};
			}
			if(mit == mend)
			{
				it += Size;
				return result_t{true};
			}
			return result_t{false};
		}
		template<std::size_t ISize>
		constexpr result_t operator()(const T(&value)[ISize]) const
		{
			auto beging = &value[0];
			auto end = beging + ISize - 1;
			if(auto result = (*this)(beging, end)) {
				if(beging == end)
					return result;
			}
			return result_t{false};
		}
		constexpr auto operator()(value_type) const
		{
			return match_result_t<T>(values.begin(), values.end());
		}
		template<class Regex>
		constexpr auto operator()(const value_type& value, const regex<Regex>& subrule) const
		{
			return match_result_t<T>();
		}
		constexpr match_result_t<T> operator()(const value_type& value, const regex& subrule) const
		{
			if(subrule == *this)
				return subrule(value);
			return {};
		}
		friend constexpr bool operator==(const regex& a, const regex& b) noexcept
		{
			return std::equal(a.values.begin(), a.values.end(), b.values.begin(), b.values.end());
		}
	};

	template<std::size_t max>
	struct regex_index_from_max_value
	{
		using type = std::conditional_t<
			max <= std::numeric_limits<std::uint8_t>::max(),
			std::uint8_t,
			std::conditional_t<
				max <= std::numeric_limits<std::uint16_t>::max(),
				std::uint16_t,
				std::conditional_t<
					max <= std::numeric_limits<std::uint32_t>::max(),
					std::uint32_t,
					std::size_t
				>
			>
		>;
	};

	template<std::size_t max>
	using regex_index_from_max_value_v = typename regex_index_from_max_value<max>::type;

	template<class T, std::size_t Size>
	struct match_anyof_t {};

	template<class T, std::size_t Size>
	class regex<match_anyof_t<T, Size>>
	{
		using index_t = regex_index_from_max_value_v<Size>;
		std::array<T, Size> values;
	public:
		template<index_t... I>
		constexpr regex(const T(&array)[Size + 1], std::integer_sequence<index_t, I...>) noexcept
		: values{array[I]...}
		{}
		constexpr regex(const T(&array)[Size + 1]) noexcept
		: regex(array, std::make_integer_sequence<index_t, Size>())
		{}
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		using input_type = T;
		using value_type = index_t;
		struct result_t
		{
			value_type value = Size;
			constexpr operator bool() const noexcept
			{
				return value < Size;
			}
			constexpr value_type operator*() const noexcept
			{
				return value;
			}
		};
	public:
		template<class Begin, class End>
		constexpr result_t operator()(Begin& it, End end) const
		{
			const auto mbegin = values.begin();
			const auto mend = values.end();
			if(it != end)
			{
				for(index_t i = 0; i < Size; ++i) {
					if(values[i] == *it) {
						it += 1;
						return result_t{i};
					}
				}
			}
			return result_t{};
		}
		template<std::size_t ISize>
		constexpr result_t operator()(const T(&value)[ISize]) const
		{
			auto beging = &value[0];
			auto end = beging + ISize - 1;
			if(auto result = (*this)(beging, end)) {
				if(beging == end)
					return result;
			}
			return result_t{};
		}
		constexpr auto operator()(value_type index) const
		{
			match_result_t<T> result;
			result.insert(result.end(), values[index]);
			return result;
		}
		template<class Regex>
		constexpr auto operator()(const value_type& value, const regex<Regex>& subrule) const
		{
			return match_result_t<T>();
		}
		constexpr match_result_t<T> operator()(const value_type& value, const regex& subrule) const
		{
			if(subrule == *this)
				return subrule(value);
			return {};
		}
		friend constexpr bool operator==(const regex& a, const regex& b) noexcept
		{
			if(a.values.size() != b.values.size())
				return false;
			const auto mbegin = b.values.data();
			const auto mend = mbegin + b.values.size();
			for(std::size_t i = 0; i < a.values.size(); ++i)
				if(std::find(mbegin, mend, a.values[i]) == mend)
					return false;
			return true;
		}
	};

	template<class T>
	struct match_range_t;

	template<class T>
	class regex<match_range_t<T>>
	{
		T from, to;
	public:
		constexpr regex(T from, T to) noexcept
		: from(std::move(from)), to(std::move(to))
		{}
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		using input_type = T;
		using value_type = T;
		using result_t = std::optional<T>;
	public:
		template<class Begin, class End>
		constexpr result_t operator()(Begin& it, End end) const
		{
			if(it != end)
			{
				if(T symbol = *it; symbol >= from && symbol <= to)
				{
					it += 1;
					return std::make_optional<value_type>(std::move(symbol));
				}
			}
			return std::nullopt;
		}
		template<std::size_t Size>
		constexpr auto operator()(const char(&value)[Size]) const
		{
			auto beging = &value[0];
			auto end = beging + Size - 1;
			if(auto result = (*this)(beging, end)) {
				if(beging == end)
					return result;
			}
			return std::nullopt;
		}
		constexpr auto operator()(const value_type& value) const
		{
			match_result_t<T> result;
			result.insert(result.end(), value);
			return result;
		}
		template<class Regex>
		constexpr auto operator()(const value_type& value, const regex<Regex>& subrule) const
		{
			return match_result_t<T>();
		}
		constexpr match_result_t<T> operator()(const value_type& value, const regex& subrule) const
		{
			if(subrule == *this)
				return subrule(value);
			return {};
		}
		friend constexpr bool operator==(const regex& a, const regex& b) noexcept
		{
			return a.from == b.from && a.to == b.to;
		}
	};

	struct match_t
	{
		template<class T, std::size_t Size>
		constexpr auto operator()(const T(&value)[Size]) const noexcept
		{
			return regex<match_sequence_t<T, Size - 1>>(value);
		}
		template<class T, std::size_t Size>
		constexpr auto operator[](const T(&value)[Size]) const noexcept
		{
			return regex<match_anyof_t<T, Size - 1>>(value);
		}
		template<class T>
		constexpr auto operator()(T from, T to) const noexcept
		{
			if(from < to)
				return regex<match_range_t<T>>(from, to);
			return regex<match_range_t<T>>(to, from);
		}
		
		match_any_t any;
	} constexpr match;

	template<class ...Regexs>
	struct match_or_t;

	namespace details
	{
		template<class ARegexs, class BRegexs>
		class regex_matchs_concat;
		template<class ARegexs, class BRegexs>
		struct visitor;
	}

	template<class ...Regexs, std::size_t... Is>
	class regex<match_or_t<std::tuple<Regexs...>, std::index_sequence<Is...>>>
	{
		using tuple = std::tuple<Regexs...>;
		template<std::size_t I>
		using tuple_value_type = typename std::tuple_element_t<I, tuple>::value_type;
	public:
		using value_type = std::variant<std::monostate, type_with_tag<tuple_value_type<Is>, tag_v<Is>>...>;
	};

	template<class ...Regexs>
	class regex<match_or_t<Regexs...>>
	{
		template<class ARegexs, class BRegexs>
		friend class details::regex_matchs_concat;
		template<class ARegexs, class BRegexs>
		friend class details::visitor;
		
		std::tuple<Regexs...> matchs;
	public:
		constexpr regex(regex&&) = default;
		constexpr regex(const regex&) = default;
		constexpr regex& operator=(const regex&) = default;
		constexpr regex& operator=(regex&&) = default;
	public:
		template<class ...IRegexs>
		constexpr regex(tag_t<from_params>, IRegexs&& ...matchs) noexcept(std::is_nothrow_constructible_v<std::tuple<Regexs...>, IRegexs...>)
		: matchs(std::forward<IRegexs>(matchs)...)
		{}
		using value_type = typename regex<match_or_t<std::tuple<Regexs...>, std::make_index_sequence<sizeof...(Regexs)>>>::value_type;
		class result_t: public value_type
		{
		public:
			using value_type::value_type;

			result_t(std::nullopt_t) noexcept
			: value_type(std::monostate{})
			{}

			operator bool() const noexcept
			{
				return value_type::index() == 0;
			}
			const result_t* operator*() const noexcept
			{
				return this;
			}
			result_t* operator*() noexcept
			{
				return this;
			}
			const result_t* operator->() const noexcept
			{
				return this;
			}
			result_t* operator->() noexcept
			{
				return this;
			}
		};
	private:
		template<class Begin, class End, std::size_t I>
		constexpr result_t operator()(Begin& it, End end, const std::index_sequence<I>&) const
		{
			if(auto result = std::get<I>(matchs)(it, end))
				return std::make_optional<value_type>(std::in_place_index<I>, std::move(*result));
			return std::nullopt;
		}
		template<class Begin, class End, std::size_t I, std::size_t... Is>
		constexpr result_t operator()(Begin& it, End end, const std::index_sequence<I, Is...>&) const
		{
			if(auto result = std::get<I>(matchs)(it, end))
				return std::make_optional<value_type>(std::in_place_index<I>, std::move(*result));
			return (*this)(it, end, std::index_sequence<Is...>());
		}
	public:
		template<class Begin, class End>
		constexpr auto operator()(Begin& it, End end) const
		{
			return (*this)(it, end, std::make_index_sequence<sizeof...(Regexs)>());
		}
		template<class T, std::size_t Size>
		constexpr result_t operator()(const T(&value)[Size]) const
		{
			auto beging = &value[0];
			auto end = beging + Size - 1;
			if(auto result = (*this)(beging, end)) {
				if(beging == end)
					return result;
			}
			return std::nullopt;
		}
	public:
		template<class value_type, std::size_t I>
		auto operator()(const type_with_tag<value_type, tag_v<I>>& value) const
		{
			return std::get<I>(matchs)(value);
		}
	public:
		constexpr auto operator()(const value_type& value) const
		{
			return std::visit(*this, value);
		}
		template<class Regex>
		constexpr auto operator()(const value_type& value, const regex<Regex>& subrule) const
		{
			return std::visit(details::visitor<match_or_t<Regexs...>, Regex>{*this, subrule}, value);
		}
		constexpr auto operator()(const value_type& value, const regex& subrule) const
		{
			using match_result_t = decltype(subrule(value));
			if(subrule == *this)
				return subrule(value);
			return match_result_t{};
		}
		friend constexpr bool operator==(const regex& a, const regex& b) noexcept
		{
			return a.matchs == b.matchs;
		}
	};

	namespace details {

		template<class ...ARegexs, class BRegex>
		struct visitor<match_or_t<ARegexs...>, BRegex>
		{
			const regex<match_or_t<ARegexs...>>& parent;
			const regex<BRegex>& subrule;

			template<class value_type, std::size_t I>
			constexpr auto operator()(const type_with_tag<value_type, tag_v<I>>& value) const
			{
				const auto& match = std::get<I>(parent.matchs);
				return match(static_cast<const value_type&>(value), subrule);
			}
		};

		template<class ...ARegexs, class ...BRegex>
		struct visitor<match_or_t<ARegexs...>, match_or_t<BRegex...>>
		{
			const regex<match_or_t<ARegexs...>>& parent;
			const regex<match_or_t<BRegex...>>& subrule;

			template<class T, class ...Ts, std::size_t I, std::size_t... Is>
			constexpr static auto subtuple(const std::tuple<Ts...>& tuple, std::index_sequence<I, Is...>) noexcept
			{
				using Head = std::tuple_element_t<I, std::tuple<Ts...>>;
				if constexpr(std::is_same_v<T, Head>) {
					if constexpr(sizeof...(Is)) {
						return std::tuple_cat(std::tie(std::get<I>(tuple)), subtuple<T>(tuple, std::index_sequence<Is...>{}));
					} else {
						return std::tie(std::get<I>(tuple));
					}
				} else {
					if constexpr(sizeof...(Is)) {
						return subtuple<T>(tuple, std::index_sequence<Is...>{});
					} else {
						return std::make_tuple();
					}
				}
			}

			template<class T, class ...Ts>
			constexpr static auto subtuple(const std::tuple<Ts...>& tuple) noexcept
			{
				return subtuple<T>(tuple, std::make_index_sequence<sizeof...(Ts)>{});
			}

			template<class Value, class Match, class Tuple, std::size_t I, std::size_t... Is>
			constexpr auto forsubrules(const Value& value, const Match& match, const Tuple& subrules, std::index_sequence<I, Is...>) const
			{
				auto head = match(value, std::get<I>(subrules));
				if constexpr(sizeof...(Is)) {
					auto tail = forsubrules(value, match, subrules, std::index_sequence<Is...>{});
					head.insert(head.end(), std::make_move_iterator(tail.begin()), std::make_move_iterator(tail.end()));
					return head;
				} else {
					return head;
				}
			}

			template<class Value, class Match, class ...Ts>
			constexpr auto forsubrules(const Value& value, const Match& match, const std::tuple<Ts...>& subrules) const
			{
				return forsubrules(value, match, subrules, std::make_index_sequence<sizeof...(Ts)>{});
			}

			template<class Value, class Match>
			constexpr auto forsubrules(const Value& value, const Match& match, const std::tuple<>&) const
			{
				using match_result_t = decltype(match(value));
				return match_result_t{};
			}

			template<class Value, std::size_t I>
			constexpr auto operator()(const type_with_tag<Value, tag_v<I>>& value) const
			{
				const auto& match = std::get<I>(parent.matchs);
				const auto& subrules = subtuple<std::tuple_element_t<I, std::tuple<ARegexs...>>>(subrule.matchs);
				return forsubrules(static_cast<const Value&>(value), match, subrules);
			}
		};

	}

	template<class ...Regexs>
	struct match_and_t;

	template<class ...Regexs>
	class regex<match_and_t<Regexs...>>
	{
		template<class ARegexs, class BRegexs>
		friend class details::regex_matchs_concat;
		
		std::tuple<Regexs...> matchs;
	public:
		regex(regex&&) = default;
		regex(const regex&) = default;
		regex& operator=(const regex&) = default;
		regex& operator=(regex&&) = default;
	public:
		template<class ...IRegexs>
		constexpr regex(tag_t<from_params>, IRegexs&& ...matchs) noexcept(std::is_nothrow_constructible_v<std::tuple<Regexs...>, IRegexs...>)
		: matchs(matchs...)
		{}
		using value_type = std::tuple<typename Regexs::value_type...>;
		using result_t = std::optional<value_type>;
	private:
		template<class Begin, class End, std::size_t... Is>
		constexpr result_t operator()(Begin& it, End end, const std::index_sequence<Is...>&) const
		{
			std::tuple results = {std::get<Is>(matchs)(it, end)...};
			if((std::get<Is>(results) && ...)) {
				return std::make_tuple(*std::move(std::get<Is>(results))...);
			}
			return std::nullopt;
		}
	public:
		template<class Begin, class End>
		constexpr auto operator()(Begin& it, End end) const
		{
			return (*this)(it, end, std::make_index_sequence<sizeof...(Regexs)>());
		}
	public:
		template<class T, std::size_t Size>
		constexpr result_t operator()(const T(&value)[Size]) const
		{
			auto beging = &value[0];
			auto end = beging + Size - 1;
			if(auto result = (*this)(beging, end)) {
				if(beging == end)
					return result;
			}
			return std::nullopt;
		}

		template<std::size_t... Is>
		constexpr auto operator()(const value_type& value, const std::index_sequence<Is...>&) const
		{
			std::array results = {(std::get<Is>(matchs)(std::get<Is>(value)))...};
			auto& result = results[0];
			for(std::size_t i = 1; i < results.size(); ++i) {
				result.insert(result.end(), std::make_move_iterator(results[i].begin()), std::make_move_iterator(results[i].end()));
			}
			return std::move(result);
		}
		constexpr auto operator()(const value_type& value) const
		{
			return (*this)(value, std::make_index_sequence<sizeof...(Regexs)>());
		}
		template<class Regex, std::size_t I>
		constexpr auto operator()(const value_type& value, const regex<Regex>& subrule, const std::index_sequence<I>&) const
		{
			return std::get<I>(matchs)(std::get<I>(value), subrule);
		}
		template<class Regex, std::size_t I, std::size_t... Is>
		constexpr auto operator()(const value_type& value, const regex<Regex>& subrule, const std::index_sequence<I, Is...>&) const
		{
			auto head = std::get<I>(matchs)(std::get<I>(value), subrule);
			auto tail = (*this)(value, subrule, std::index_sequence<Is...>{});
			head.insert(head.end(), std::make_move_iterator(tail.begin()), std::make_move_iterator(tail.end()));
			return head;
		}
		template<class Regex>
		constexpr auto operator()(const value_type& value, const regex<Regex>& subrule) const
		{
			return (*this)(value, subrule, std::make_index_sequence<sizeof...(Regexs)>());
		}
		constexpr auto operator()(const value_type& value, const regex& subrule) const
		{
			using match_result_t = decltype(subrule(value));
			if(subrule == *this)
				return subrule(value);
			return match_result_t{};
		}
		friend constexpr bool operator==(const regex& a, const regex& b) noexcept
		{
			return a.matchs == b.matchs;
		}
	};

	namespace details
	{
		template<class ...ARegexs, class ...BRegexs>
		class regex_matchs_concat<std::tuple<ARegexs...>, std::tuple<BRegexs...>>
		{
		private:
			template<std::size_t... Ia, std::size_t... Ib>
			static constexpr auto concat(regex<match_or_t<ARegexs...>>&& a, const std::index_sequence<Ia...>&, regex<match_or_t<BRegexs...>>&& b, const std::index_sequence<Ib...>&) noexcept
			{
				return regex<match_or_t<ARegexs..., BRegexs...>>(tag<from_params>, std::move(std::get<Ia>(a.matchs))..., std::move(std::get<Ib>(b.matchs))...);
			}
		public:
			static constexpr auto concat(regex<match_or_t<ARegexs...>> a, regex<match_or_t<BRegexs...>> b) noexcept
			{
				return concat(std::move(a), std::make_index_sequence<sizeof...(ARegexs)>(), std::move(b), std::make_index_sequence<sizeof...(BRegexs)>());
			}
		private:
			template<std::size_t... Ia, std::size_t... Ib>
			static constexpr auto concat(regex<match_and_t<ARegexs...>>&& a, const std::index_sequence<Ia...>&, regex<match_and_t<BRegexs...>>&& b, const std::index_sequence<Ib...>&) noexcept
			{
				return regex<match_and_t<ARegexs..., BRegexs...>>(tag<from_params>, std::move(std::get<Ia>(a.matchs))..., std::move(std::get<Ib>(b.matchs))...);
			}
		public:
			static constexpr auto concat(regex<match_and_t<ARegexs...>> a, regex<match_and_t<BRegexs...>> b) noexcept
			{
				return concat(std::move(a), std::make_index_sequence<sizeof...(ARegexs)>(), std::move(b), std::make_index_sequence<sizeof...(BRegexs)>());
			}
		};
	}

	template<class RegexA, class RegexB>
	constexpr auto operator|(regex<RegexA> a, regex<RegexB> b) noexcept
	{
		return regex<match_or_t<regex<RegexA>, regex<RegexB>>>(tag<from_params>, std::move(a), std::move(b));
	}
	template<class Regex>
	constexpr auto operator|(regex<Regex> a, const match_any_t&) noexcept
	{
		using any_t = regex<match_any<typename regex<Regex>::input_type>>;
		return regex<match_or_t<regex<Regex>, any_t>>(tag<from_params>, std::move(a), any_t{});
	}
	template<class Regex>
	constexpr auto operator|(const match_any_t&, regex<Regex>) noexcept
	{
		using any_t = regex<match_any<typename regex<Regex>::input_type>>;
		return any_t{};
	}
	template<class ...Regexs, class T, class RegexB>
	constexpr auto operator|(regex<match_or_t<Regexs..., regex<match_any<T>>>> a, regex<RegexB>) noexcept
	{
		return std::move(a);
	}
	template<class ...Regexs, class RegexB>
	constexpr auto operator|(regex<match_or_t<Regexs...>> a, regex<RegexB> b) noexcept
	{
		return std::move(a) | regex<match_or_t<regex<RegexB>>>(tag<from_params>, std::move(b));
	}
	template<class RegexA, class ...Regexs>
	constexpr auto operator|(regex<RegexA> a, regex<match_or_t<Regexs...>> b) noexcept
	{
		return regex<match_or_t<regex<RegexA>>>(tag<from_params>, std::move(a)) | std::move(b);
	}
	template<class ...ARegexs, class ...BRegexs>
	constexpr auto operator|(regex<match_or_t<ARegexs...>> a, regex<match_or_t<BRegexs...>> b) noexcept
	{
		return details::regex_matchs_concat<std::tuple<ARegexs...>, std::tuple<BRegexs...>>::concat(std::move(a), std::move(b));
	}
	template<class T, std::size_t RSize, std::size_t ISize>
	constexpr auto operator|(const regex<match_anyof_t<T, RSize>>& a, const T(&b)[ISize]) noexcept
	{
		return a | match[b];
	}
	template<class T, std::size_t RSize, std::size_t ISize>
	constexpr auto operator|(const char(&a)[ISize], const regex<match_anyof_t<T, RSize>>& b) noexcept
	{
		return match[a] | b;
	}
	template<class RegexA, class RegexB>
	constexpr auto operator&(regex<RegexA> a, regex<RegexB> b) noexcept
	{
		return regex<match_and_t<regex<RegexA>, regex<RegexB>>>(tag<from_params>, std::move(a), std::move(b));
	}
	template<class Regex>
	constexpr auto operator&(regex<Regex> a, const match_any_t&) noexcept
	{
		using any_t = regex<match_any<typename regex<Regex>::input_type>>;
		return regex<match_and_t<regex<Regex>, any_t>>(tag<from_params>, std::move(a), any_t{});
	}
	template<class Regex>
	constexpr auto operator&(const match_any_t&, regex<Regex> a) noexcept
	{
		using any_t = regex<match_any<typename regex<Regex>::input_type>>;
		return regex<match_and_t<any_t, regex<Regex>>>(tag<from_params>, any_t{}, std::move(a));
	}
	template<class ...Regexs, class RegexB>
	constexpr auto operator&(regex<match_and_t<Regexs...>> a, regex<RegexB> b) noexcept
	{
		return std::move(a) & regex<match_and_t<regex<RegexB>>>(tag<from_params>, std::move(b));
	}
	template<class RegexA, class ...Regexs>
	constexpr auto operator&(regex<RegexA> a, regex<match_and_t<Regexs...>> b) noexcept
	{
		return regex<match_and_t<regex<RegexA>>>(tag<from_params>, std::move(a)) & std::move(b);
	}
	template<class ...ARegexs, class ...BRegexs>
	constexpr auto operator&(regex<match_and_t<ARegexs...>> a, regex<match_and_t<BRegexs...>> b) noexcept
	{
		return details::regex_matchs_concat<std::tuple<ARegexs...>, std::tuple<BRegexs...>>::concat(std::move(a), std::move(b));
	}
	template<class T, std::size_t RSize, std::size_t ISize>
	constexpr auto operator&(const regex<match_sequence_t<T, RSize>>& a, const T(&b)[ISize]) noexcept
	{
		return a & match(b);
	}
	template<class T, std::size_t RSize, std::size_t ISize>
	constexpr auto operator&(const T(&a)[ISize], const regex<match_sequence_t<T, RSize>>& b) noexcept
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
		using value_type = std::vector<typename Match::value_type>;
		using result_t = std::optional<value_type>;
	public:
		template<class Begin, class End>
		constexpr result_t operator()(Begin& it, End end) const
		{
			value_type results;
			while(auto result = match(it, end)) {
				results.emplace_back(*std::move(result));
			}
			return std::make_optional<value_type>(std::move(results));
		}
		template<class T, std::size_t Size>
		constexpr result_t operator()(const T(&value)[Size]) const
		{
			auto beging = &value[0];
			auto end = beging + Size - 1;
			if(auto result = (*this)(beging, end)) {
				if(beging == end)
					return result;
			}
			return std::nullopt;
		}
		constexpr auto operator()(const value_type& values) const
		{
			decltype(match(*values.begin())) results;
			for(const auto& value: values) {
				auto result = match(value);
				results.insert(results.end(), std::make_move_iterator(result.begin()), std::make_move_iterator(result.end()));
			}
			return results;
		}
		template<class Regex>
		constexpr auto operator()(const value_type& values, const regex<Regex>& subrule) const
		{
			decltype(match(*values.begin())) results;
			for(const auto& value: values) {
				auto result = match(value, subrule);
				results.insert(results.end(), std::make_move_iterator(result.begin()), std::make_move_iterator(result.end()));
			}
			return results;
		}
		constexpr auto operator()(const value_type& value, const regex& subrule) const
		{
			if(subrule == *this)
				return subrule(value);
			return {};
		}
		friend constexpr bool operator==(const regex& a, const regex& b) noexcept
		{
			return a.match == b.match;
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
		using value_type = typename Match::result_t;
		using result_t = std::optional<value_type>;
	public:
		template<class Begin, class End>
		constexpr result_t operator()(Begin& it, End end) const
		{
			return std::make_optional<value_type>(match(it, end));
		}
		template<class T, std::size_t Size>
		constexpr result_t operator()(const T(&value)[Size]) const
		{
			auto beging = &value[0];
			auto end = beging + Size - 1;
			if(auto result = (*this)(beging, end)) {
				if(beging == end)
					return result;
			}
			return std::nullopt;
		}
		constexpr auto operator()(const value_type& value) const
		{
			using result_t = decltype(match(*value));
			if(value) {
				return match(*value);
			}
			return result_t{};
		}
		friend constexpr bool operator==(const regex& a, const regex& b) noexcept
		{
			return a.match == b.match;
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
		using value_type = std::vector<typename Match::value_type>;
		using result_t = std::optional<value_type>;
	public:
		template<class Begin, class End>
		constexpr result_t operator()(Begin& it, End end) const
		{
			value_type results;
			if(auto result = match(it, end))
			{
				results.emplace_back(*std::move(result));
				while(auto result = match(it, end)) {
					results.emplace_back(*std::move(result));
				}
				return std::make_optional<value_type>(std::move(results));
			}
			return std::nullopt;
		}
		template<class T, std::size_t Size>
		constexpr auto operator()(const T(&value)[Size]) const
		{
			auto beging = &value[0];
			auto end = beging + Size - 1;
			if(auto result = (*this)(beging, end)) {
				if(beging == end)
					return result;
			}
			return std::nullopt;
		}
		constexpr auto operator()(const value_type& values) const
		{
			decltype(match(values)) results;
			for(const auto& value: values) {
				auto result = match(value);
				results.insert(results.end(), std::make_move_iterator(result.begin()), std::make_move_iterator(result.end()));
			}
			return results;
		}
		friend constexpr bool operator==(const regex& a, const regex& b) noexcept
		{
			return a.match == b.match;
		}
	};

	template<class Match>
	constexpr auto operator +(regex<Match> match) noexcept
	{
		return regex<match_one_plus<regex<Match>>>(std::move(match));
	}

	template<class Match, class T, std::size_t ISize>
	constexpr auto parse(const regex<Match>& regex, const T(&value)[ISize])
	{
		auto beging = &value[0];
		auto end = beging + ISize - 1;
		return regex(beging, end);
	}

	template<class Match, class Value>
	constexpr auto generate(const regex<Match>& regex, const Value& value)
	{
		return regex(value);
	}

}