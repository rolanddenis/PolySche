#pragma once

#include <iostream>
#include <numeric>
#include <type_traits>
#include <cmath>
#include <array>
#include <cassert>

/**
 * Constexpr implementation of rational numbers (fraction of signed integers)
 * 
 * This implementation is not meant for efficiency but instead to be used occasionally or at compile time.
 * Each operation on a Rational is designed to avoid overflow and will return reduced rational.
 */

template <typename T>
struct Rational
{
    using value_type = T;
    static_assert(std::is_signed_v<value_type>, "Rational integer type must be signed");

    value_type p, q;

    constexpr Rational(T p = 0, T q = 1) : p(p), q(q) {}

    constexpr bool is_valid() const noexcept
    {
        return q != 0;
    }

    constexpr Rational<T> simplify() const noexcept
    {
        assert(is_valid() && "Not valid rational");
        T qq = (q == 0) ? T(1) : q; 

        // Rational should be valid
        T g = std::gcd(p, qq);
        T pp = p / g;
        qq = q / g;
        if (qq < 0)
        {
            qq = -qq;
            pp = -pp;
        }
        return Rational<T>(pp, qq);
    }

    template <typename U>
    constexpr operator U () const noexcept
    {
        return static_cast<U>(p) / static_cast<U>(q);
    }

};

// Type traits
template <typename T>
struct IsRational : std::false_type {};

template <typename T>
struct IsRational<Rational<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_rational_v = IsRational<std::decay_t<T>>::value;

template <typename T>
using rational_value_t = typename std::decay_t<T>::value_type;

namespace std
{
template <typename T, typename U>
struct common_type<Rational<T>, U> { using type = Rational<std::make_signed_t<std::common_type_t<T, U>>>; };

template <typename T, typename U>
struct common_type<U, Rational<T>> { using type = Rational<std::make_signed_t<std::common_type_t<T, U>>>; };

template <typename T, typename U>
struct common_type<Rational<T>, Rational<U>> { using type = Rational<std::common_type_t<T, U>>; };
}

/// Helper to reduce code duplication in binary operations
template <typename T>
constexpr auto as_rational(T && v) noexcept -> decltype(auto)
{
    if constexpr (is_rational_v<T>)
        return std::forward<T>(v);
    else
    {
        static_assert(std::is_integral_v<std::decay_t<T>>, "Cannot convert to a Rational");
        using type = std::make_signed_t<std::decay_t<T>>; // Ensure signed integer type
        return Rational<type>{static_cast<type>(v)};
    }
}

/// Constexpr swap of Rationals
template <typename T>
constexpr void swap(Rational<T> & lhs, Rational<T> & rhs) noexcept
{
    // std::swap isn't constexpr until C++20
    Rational<T> tmp = lhs;
    lhs = rhs;
    rhs = tmp;
}

template <
    typename LHS,
    typename RHS,
    typename = std::enable_if_t<is_rational_v<LHS> or is_rational_v<RHS>>
>
constexpr auto operator* (LHS && lhs, RHS && rhs) noexcept
{
    auto && lhs_r = as_rational(std::forward<LHS>(lhs));
    auto && rhs_r = as_rational(std::forward<RHS>(rhs));
    return Rational(lhs_r.p * rhs_r.p, lhs_r.q * rhs_r.q).simplify();
}

template <
    typename LHS,
    typename RHS,
    typename = std::enable_if_t<is_rational_v<LHS> or is_rational_v<RHS>>
>
constexpr auto operator/ (LHS && lhs, RHS && rhs) noexcept
{
    auto && lhs_r = as_rational(std::forward<LHS>(lhs));
    auto && rhs_r = as_rational(std::forward<RHS>(rhs));
    return Rational(lhs_r.p * rhs_r.q, lhs_r.q * rhs_r.p).simplify();
}

template <
    typename LHS,
    typename RHS,
    typename = std::enable_if_t<is_rational_v<LHS> or is_rational_v<RHS>>
>
constexpr auto operator+ (LHS && lhs, RHS && rhs) noexcept
{
    auto && lhs_r = as_rational(std::forward<LHS>(lhs));
    auto && rhs_r = as_rational(std::forward<RHS>(rhs));
    auto lcm = std::lcm(lhs_r.q, rhs_r.q);
    return Rational(lhs_r.p * (lcm / lhs_r.q) + rhs_r.p * (lcm / rhs_r.q), lcm).simplify();
}

template <
    typename LHS,
    typename RHS,
    typename = std::enable_if_t<is_rational_v<LHS> or is_rational_v<RHS>>
>
constexpr auto operator- (LHS && lhs, RHS && rhs) noexcept
{
    auto && lhs_r = as_rational(std::forward<LHS>(lhs));
    auto && rhs_r = as_rational(std::forward<RHS>(rhs));
    auto lcm = std::lcm(lhs_r.q, rhs_r.q);
    return Rational(lhs_r.p * (lcm / lhs_r.q) - rhs_r.p * (lcm / rhs_r.q), lcm).simplify();
}

template <
    typename LHS,
    typename RHS,
    typename = std::enable_if_t<is_rational_v<LHS> or is_rational_v<RHS>>
>
constexpr auto operator== (LHS && lhs, RHS && rhs) noexcept
{
    // It supposes that both operands are valid rational.
    auto && lhs_r = as_rational(std::forward<LHS>(lhs));
    auto && rhs_r = as_rational(std::forward<RHS>(rhs));
    auto a = lhs_r.p * rhs_r.q;
    auto b = lhs_r.q * rhs_r.p;
    return a == b;
}

template <
    typename LHS,
    typename RHS,
    typename = std::enable_if_t<is_rational_v<LHS> or is_rational_v<RHS>>
>
constexpr auto operator< (LHS && lhs, RHS && rhs) noexcept
{
    // It supposes that both operands are valid rational.
    auto && lhs_r = as_rational(std::forward<LHS>(lhs));
    auto && rhs_r = as_rational(std::forward<RHS>(rhs));
    auto a = lhs_r.p * rhs_r.q;
    auto b = lhs_r.q * rhs_r.p;
    if ((lhs_r.q < 0) xor (rhs_r.q > 0))
        return a < b;
    else
        return a > b;
}

template <
    typename LHS,
    typename RHS,
    typename = std::enable_if_t<is_rational_v<LHS> or is_rational_v<RHS>>
>
constexpr auto operator> (LHS && lhs, RHS && rhs) noexcept
{
    // It supposes that both operands are valid rational.
    auto && lhs_r = as_rational(std::forward<LHS>(lhs));
    auto && rhs_r = as_rational(std::forward<RHS>(rhs));
    auto a = lhs_r.p * rhs_r.q;
    auto b = lhs_r.q * rhs_r.p;
    if ((lhs_r.q < 0) xor (rhs_r.q > 0))
        return a > b;
    else
        return a < b;
}

template <typename T>
constexpr bool signbit(Rational<T> const& r) noexcept
{
    return not(std::signbit(r.p) xor std::signbit(r.q));
}

template <typename T>
constexpr auto abs(Rational<T> const& r) noexcept
{
    //return Rational(std::abs(r.p), std::abs(r.q)); // std::abs is not constexpr until C++23
    return Rational(
        (r.p >= 0) ? r.p : -r.p,
        (r.q >= 0) ? r.q : -r.q
    );
}

template <
    typename T,
    typename = std::enable_if_t<is_rational_v<T>>
>
constexpr auto simplify(T && r) noexcept
{
    return r.simplify();
}

template <typename T>
std::ostream& operator<< (std::ostream& out, Rational<T> const& r)
{
    auto rs = r; //r.simplify();
    out << rs.p;
    if (rs.q != 1)
        out << "/" << rs.q;
    return out;
}
