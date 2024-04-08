#include <iostream>

#include "polynomial.hpp"
#include "utils.hpp"
#include "rational.hpp"


int main()
{
    using polysche::Rational;
    using polysche::Polynomial;
    
    using T = Rational<int>;
    constexpr Polynomial<T, 2, 3> P({{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}});
    std::cout << "P = " << P << std::endl;

    constexpr auto P3 = P(3);
    std::cout << "P(3) = " << P3 << std::endl;

    constexpr auto DP1 = P.derivate();
    std::cout << "P' = " << DP1 << std::endl;

    constexpr auto DP2 = P.derivate(2);
    std::cout << "P'' = " << DP2 << std::endl;

    constexpr auto DP3 = P.derivate(3);
    std::cout << "P''' = " << DP3 << std::endl;

    constexpr auto DP4 = P.derivate(4);
    std::cout << "P'''' = " << DP4 << std::endl;

    constexpr auto DP1_int = DP1.integrate({1, 2}, {3, 2});
    std::cout << "DP1.integrate(1/2, 3/2) = " << DP1_int << std::endl;

    Polynomial<T, 2, 3> PP;
    std::cout << "PP.coeffs = " << PP.coeffs << std::endl;
    std::cout << PP.coeffs[0][0].p << " " << PP.coeffs[0][0].q << std::endl;
    return 0;
}