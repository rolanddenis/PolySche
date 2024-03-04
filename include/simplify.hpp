/// Representation simplification for different types (target type is a Rational)

#pragma once

#include <array>
#include <type_traits>

template <
    typename T,
    typename = std::enable_if_t<std::is_fundamental_v<std::decay_t<T>>>
>
constexpr auto simplify(T && v) noexcept -> decltype(auto)
{
    return std::forward<T>(v);
}

template <typename T, std::size_t N>
constexpr auto simplify(std::array<T, N> a) noexcept
{
    for (std::size_t i = 0; i < N; ++i)
        a[i] = simplify(a[i]);
    return a;
}