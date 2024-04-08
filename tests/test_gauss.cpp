#include <iostream>
#include <array>

#include "gauss.hpp"
#include "rational.hpp"
#include "utils.hpp"

int main()
{
    using polysche::Rational;
    using polysche::gauss_solve;
    using polysche::gauss_inv;
    
    using T = Rational<int>;
    constexpr std::array<std::array<T, 3>, 3> A{{{1, -1, 1}, {1, 0, 0}, {1, 1, 1}}};
    constexpr std::array<T, 3> b = {1, 0, 2};
    constexpr auto x = gauss_solve(A, b);
    std::cout << "x = " << x << std::endl;

    constexpr auto iA = gauss_inv(A);
    std::cout << "inv(A) = " << iA << std::endl;

    return 0;
}