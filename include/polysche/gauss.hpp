#pragma once

#include <array>

namespace polysche
{

// std::swap isn't constexpr until C++20
template <
    typename T,
    std::size_t N
>
constexpr void swap_array(std::array<T, N> & lhs, std::array<T, N> & rhs) noexcept
{
    std::array<T, N> tmp = lhs;
    lhs = rhs;
    rhs = tmp;
}

/// Gauss-Jordan elimination of a matrix M x N 
template <
    typename T,
    std::size_t M,
    std::size_t N
>
constexpr std::array<std::array<T, N>, M> gauss(std::array<std::array<T, N>, M> A) noexcept
{
    using std::abs;
    using std::swap;

    std::size_t r = 0;
    for (std::size_t j = 0; j < N and r < M; ++j)
    {
        std::size_t k = r;
        auto kv = A[k][j];
        for (std::size_t i = k + 1; i < M; ++i)
            if (abs(A[i][j]) > abs(kv))
            {
                k = i;
                kv = A[i][j];
            }
        
        if (kv == T(0)) continue;

        for (std::size_t jj = 0; jj < N; ++jj)
            A[k][jj] = A[k][jj] / kv;

        if (k != r)
            swap_array(A[k], A[r]);

        for (std::size_t i = 0; i < M; ++i)
        {
            if (i != r)
            {
                auto c = A[i][j];
                for (std::size_t jj = 0; jj < N; ++jj)
                    A[i][jj] = A[i][jj] - A[r][jj] * c;
            }
        }

        ++r;
    }

    return A;
}

/// Solve a linear system using Gauss-Jordan algorithm
template <
    typename T,
    std::size_t N
>
constexpr std::array<T, N> gauss_solve(std::array<std::array<T, N>, N> const& A, std::array<T, N> const& b) noexcept
{
    std::array<std::array<T, N + 1>, N> augmented_A;
    for (std::size_t i = 0; i < N; ++i)
    {
        for (std::size_t j = 0; j < N; ++j)
            augmented_A[i][j] = A[i][j];
        augmented_A[i][N] = b[i];
    }

    auto reducted_A = gauss(augmented_A);
    std::array<T, N> solution;
    for (std::size_t i = 0; i < N; ++i)
        solution[i] = reducted_A[i][N] / reducted_A[i][i];
    return solution;
}   

/// Compute the inverse of a matrix using Gauss-Jordan algorithm
template <
    typename T,
    std::size_t N
>
constexpr std::array<std::array<T, N>, N> gauss_inv(std::array<std::array<T, N>, N> const& A) noexcept
{
    std::array<std::array<T, N + N>, N> augmented_A;
    for (std::size_t i = 0; i < N; ++i)
    {
        for (std::size_t j = 0; j < N; ++j)
        {
            augmented_A[i][j] = A[i][j];
            augmented_A[i][j + N] = (i == j) ? T(1) : T(0);
        }
    }

    auto reducted_A = gauss(augmented_A);

    std::array<std::array<T, N>, N> inv;
    for (std::size_t i = 0; i < N; ++i)
        for (std::size_t j = 0; j < N; ++j)
            inv[i][j] = reducted_A[i][j + N];
    return inv;
}

} // namespace polysche
