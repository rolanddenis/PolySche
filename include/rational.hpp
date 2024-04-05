#pragma once

#include <iostream>
#include <numeric>
#include <type_traits>
#include <cmath>
#include <array>
#include <cassert>

/** @brief Constexpr implementation of rational numbers (fraction of signed integers)
 * 
 * This implementation is not meant for efficiency but instead to be used occasionally or at compile time.
 * Each operation on a Rational is designed to avoid overflow and will return reduced rational.
 * 
 * @tparam T    Value type of the numerator and the denominator of the fraction (signed integers only).
 */
template <typename T>
struct Rational
{
    using value_type = T;
    static_assert(std::is_signed_v<value_type> and std::is_arithmetic_v<value_type>, "Rational type must be of signed integer type");

    value_type p = 0, q = 1;

    constexpr Rational(T pp = 0, T qq = 1)
    {
        // Always constructing a valid and simplified rational
        assert(qq != T(0) && "Not valid rational");
        qq = (qq == T(0)) ? T(1) : qq; 

        T g = std::gcd(pp, qq);
        pp = pp / g;
        qq = qq / g;
        if (qq < T(0))
        {
            qq = -qq;
            pp = -pp;
        }
        p = pp;
        q = qq;
    }

    /// true if the denominator is not null
    constexpr bool is_valid() const noexcept
    {
        return q != T(0);
    }

    /// true if the numerator is null
    constexpr bool is_zero() const noexcept
    {
        return p == T(0);
    }

    /// Conversion to an arithmetic type (or any with a division operator)
    template <typename U>
    constexpr operator U () const noexcept
    {
        return static_cast<U>(p) / static_cast<U>(q);
    }

};

///////////////////////////////////////////////////////////////////////////////
// Type traits

template <typename T>
struct IsRational : std::false_type {};

template <typename T>
struct IsRational<Rational<T>> : std::true_type {};

/// Is true if type @c T decays to a Rational
template <typename T>
inline constexpr bool is_rational_v = IsRational<std::decay_t<T>>::value;

/// Alias to the value type of a Rational @c T
template <typename T>
using rational_value_t = typename std::decay_t<T>::value_type;

namespace std
{
/// Overload of std::common_type for a Rational and an arithmetic type
template <typename T, typename U>
struct common_type<Rational<T>, U> { using type = Rational<std::make_signed_t<std::common_type_t<T, U>>>; };

/// Overload of std::common_type for an arithmetic type and a Rational
template <typename T, typename U>
struct common_type<U, Rational<T>> { using type = Rational<std::make_signed_t<std::common_type_t<T, U>>>; };

/// Overload of std::common_type for two Rationals
template <typename T, typename U>
struct common_type<Rational<T>, Rational<U>> { using type = Rational<std::common_type_t<T, U>>; };
}

/** @brief Helper to reduce code duplication in binary operations
 * 
 * It forward given paramater "as it" if it is a Rational
 * and otherwise return the result of it's conversion to a Rational.
 * 
 * It allows generalize binary operations between a Rational and another type
 * (Rational or arithmetic) without duplicating code for each combinaison.
 */
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

/// Product
template <
    typename LHS,
    typename RHS,
    typename = std::enable_if_t<is_rational_v<LHS> or is_rational_v<RHS>>
>
constexpr auto operator* (LHS && lhs, RHS && rhs) noexcept
{
    auto && lhs_r = as_rational(std::forward<LHS>(lhs));
    auto && rhs_r = as_rational(std::forward<RHS>(rhs));
    return Rational(lhs_r.p * rhs_r.p, lhs_r.q * rhs_r.q);
}

/// Division
template <
    typename LHS,
    typename RHS,
    typename = std::enable_if_t<is_rational_v<LHS> or is_rational_v<RHS>>
>
constexpr auto operator/ (LHS && lhs, RHS && rhs) noexcept
{
    auto && lhs_r = as_rational(std::forward<LHS>(lhs));
    auto && rhs_r = as_rational(std::forward<RHS>(rhs));
    return Rational(lhs_r.p * rhs_r.q, lhs_r.q * rhs_r.p);
}

/// Addition
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
    return Rational(lhs_r.p * (lcm / lhs_r.q) + rhs_r.p * (lcm / rhs_r.q), lcm);
}

/// Subtraction
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
    return Rational(lhs_r.p * (lcm / lhs_r.q) - rhs_r.p * (lcm / rhs_r.q), lcm);
}

/// Equal to
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

/// Lower than
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

/// Greater than
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

/// Sign bit of a Rational (true if negative, false otherwise)
template <typename T>
constexpr bool signbit(Rational<T> const& r) noexcept
{
    return not(std::signbit(r.p) xor std::signbit(r.q));
}

/// Absolute value of a Rational
template <typename T>
constexpr auto abs(Rational<T> const& r) noexcept
{
    //return Rational(std::abs(r.p), std::abs(r.q)); // std::abs is not constexpr until C++23
    return Rational(
        (r.p >= 0) ? r.p : -r.p,
        (r.q >= 0) ? r.q : -r.q
    );
}

/// Representation of a Rational
template <typename T>
std::ostream& operator<< (std::ostream& out, Rational<T> const& r)
{
    auto rs = r; //r.simplify();
    out << rs.p;
    if (rs.q != 1)
        out << "/" << rs.q;
    return out;
}
