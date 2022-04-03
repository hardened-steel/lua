#pragma once
#include <array>
#include <type_traits>
#include <tuple>

namespace utils::types {

    template<class T, class Tuple, std::size_t I, std::size_t ...Is>
    constexpr auto get_elements(Tuple& tuple, std::index_sequence<I, Is...>) noexcept
    {
        using tuple_t = std::remove_const_t<std::remove_reference_t<Tuple>>;
        if constexpr (std::is_same_v<std::tuple_element_t<I, tuple_t>, T>) {
            if constexpr(sizeof...(Is) > 0) {
                return std::tuple_cat(std::make_tuple(&std::get<I>(tuple)), get_elements<T>(tuple, std::index_sequence<Is...>{}));
            } else {
                return std::make_tuple(&std::get<I>(tuple));
            }
        } else {
            if constexpr(sizeof...(Is) > 0) {
                return get_elements<T>(tuple, std::index_sequence<Is...>{});
            } else {
                return std::make_tuple();
            }
        }
    }

    namespace details {

        template<class T, class Tuple, std::size_t ...Is>
        constexpr auto get_array(Tuple& tuple, std::index_sequence<Is...>) noexcept
        {
            return std::array{std::get<Is>(tuple)...};
        }

        template<class T, class Tuple>
        constexpr auto get_array(Tuple&& tuple) noexcept
        {
            return get_array<T>(tuple, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
        }

    }

    template<class T, class Tuple>
    constexpr auto get_elements(Tuple& tuple) noexcept
    {
        using tuple_t = std::remove_const_t<std::remove_reference_t<Tuple>>;
        return details::get_array<T>(get_elements<T>(tuple, std::make_index_sequence<std::tuple_size_v<tuple_t>>{}));
    }

}