#pragma once

#include <array>

#include "rational.hpp"
#include "polynomial.hpp"
#include "gauss.hpp"

template <
    std::size_t Order,
    typename T = Rational<long long int>
>
struct PolynomialScheme
{
    std::array<std::array<T, Order + 1>, Order + 1> matrix;
    std::size_t index = 0;

    constexpr auto get_polynomial() const noexcept
    {
        Polynomial<T, Order, Order + 1> P{};
        for (std::size_t degree = 0; degree < Order + 1; ++degree)
            P.coeffs[degree][degree] = T(1);
        return P;
    }

    constexpr auto add_eqn(std::array<T, Order + 1> const& coeffs) const noexcept
    {
        auto PS = *this;
        PS.matrix[index] = coeffs;
        ++PS.index;
        return PS;
    }

    constexpr auto solve() const noexcept
    {
        Polynomial<T, Order, Order + 1> P{};
        P.coeffs = gauss_inv(matrix);
        return P;
    }
};