#pragma once

#include <array>
#include <iostream>

namespace std
{

template <typename T, std::size_t N>
std::ostream & operator<< (std::ostream & out, std::array<T, N> const& a)
{
    out << "[";
    for (std::size_t i = 0; i < N; ++i)
        out << a[i] << ((i < N - 1) ? "," : "");
    out << "]";
    return out;
}

} // namespace std
