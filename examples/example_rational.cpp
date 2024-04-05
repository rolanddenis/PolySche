#include <iostream>

#include "rational.hpp"

int main()
{
    using T = int;
    constexpr Rational<T> a{3, 2};
    std::cout << "a = " << a << std::endl;
    std::cout << "is_rational_v(a) = " << is_rational_v<decltype(a)> << std::endl;

    constexpr auto a3r = a * 3;
    std::cout << "a * 3 = " << a3r << std::endl;

    constexpr auto a3l = 3 * a;
    std::cout << "3 * a = " << a3l << std::endl;

    constexpr Rational<T> b{3};
    std::cout << "b = " << b << std::endl;

    constexpr Rational<T> c{-1, 10};
    std::cout << "c = " << c << std::endl;

    constexpr auto ac = a * c;
    std::cout << "a * c = " << ac << std::endl;

    constexpr auto d = Rational(2ll, -20ll);
    std::cout << "d = " << d << std::endl;
    std::cout << "d as double = " << static_cast<double>(d) << std::endl;

    using std::signbit;
    constexpr bool ds = signbit(d);
    std::cout << "signbit(d) = " << ds << std::endl;

    using std::abs;
    constexpr auto da = abs(d);
    std::cout << "abs(d) = " << da << std::endl;

    std::cout << "d * a == c * a ? " <<  ((d * a) == (c * a)) << std::endl;

    constexpr bool cmp_ad = a < d;
    std::cout << "a < d ? " << cmp_ad << std::endl;

    constexpr bool cmp_da = d < a;
    std::cout << "d < a ? " << cmp_da << std::endl;

    constexpr bool cmp_d2 = d < 2;
    std::cout << "d < 2 ? " << cmp_d2 << std::endl;

    constexpr auto dummy = (a + b) * c + d;
    std::cout << "dummy = " << dummy << std::endl;

    constexpr std::size_t i = 3;
    constexpr auto ri = i * dummy;
    std::cout << "ri = " << ri  << std::endl;
    constexpr Rational<T> rit = ri;
    std::cout << "rit = " << rit << std::endl;



    return 0;
}