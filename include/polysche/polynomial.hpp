#pragma once

#include <array>
#include <type_traits>
#include <iostream>

namespace polysche
{

template <
    typename T,
    std::size_t Degree,
    std::size_t N
>
struct Polynomial
{
    std::array<std::array<T, N>, Degree + 1> coeffs;

    constexpr Polynomial(T const& value = T(0))
    {
        for (auto & row : coeffs)
            for (auto & c : row)
                c = value;
    }
    

    constexpr Polynomial(std::array<std::array<T, N>, Degree + 1> const& coeffs) : coeffs(coeffs) {}

    template <typename X>
    constexpr auto operator() (X && x) const noexcept
    {
        using Result = std::common_type_t<T, std::decay_t<X>>;
        
        std::array<Result, N> result;
        for (std::size_t i = 0; i < N; ++i)
            result[i] = coeffs[0][i];
            
        auto xi = x;
        for (std::size_t degree = 1; degree < Degree + 1; ++degree)
        {
            for (std::size_t i = 0; i < N; ++i)
            {
                result[i] = result[i] + coeffs[degree][i] * xi;
            }
            xi = xi * x;
        }
        return result;
    }

    constexpr auto derivate(std::size_t order = 1) const noexcept
    {
        if (order == 0 or Degree == 0)
            return *this;
        else if (Degree == 1)
        {
            return Polynomial<T, Degree, N>{};
        }

        Polynomial<T, Degree, N> P{};
        for (std::size_t degree = 1; degree < Degree + 1; ++degree)
            for (std::size_t i = 0; i < N; ++i)
                P.coeffs[degree - 1][i] = degree * coeffs[degree][i];
        return P.derivate(order - 1);
    }

    constexpr auto primitive() const noexcept
    {
        Polynomial<T, Degree + 1, N> P{};
        for (std::size_t degree = 1; degree <= Degree + 1; ++degree)
            for (std::size_t i = 0; i < N; ++i)
                P.coeffs[degree][i] = coeffs[degree - 1][i] / degree;
        return P;
    }

    constexpr auto integrate(T const& a, T const& b) const noexcept
    {
        auto P = primitive();
        auto Pa = P(a);
        auto Pb = P(b);

        std::array<T, N> result{};
        for (std::size_t i = 0; i < N; ++i)
            result[i] = Pb[i] - Pa[i];
        return result;
    }
};

template <
    typename T,
    std::size_t Degree,
    std::size_t N
>
std::ostream & operator<< (std::ostream & out, Polynomial<T, Degree, N> const& P)
{
    for (std::size_t degree = 0; degree <= Degree; ++degree)
    {
        out << P.coeffs[degree];
        if (degree == 1)
            out << " X";
        else if (degree > 1)
            out << " X^" << degree;
        if (degree < Degree)
            out << " + ";
    }
    return out;
}

} // namespace polysche
