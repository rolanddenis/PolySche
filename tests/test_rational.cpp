#include "rational.hpp"
#include "utils.hpp"

int main()
{
    using T = int;
    using polysche::Rational;
    using polysche::is_rational_v;

    // Construction from an integer
    {
    constexpr Rational<T> a(3);
    CHECK(a.is_valid() && !a.is_zero());
    CHECK(is_rational_v<decltype(a)>);
    CHECK(a.p == 3 && a.q == 1);
    CHECK(static_cast<double>(a) == 3.);
    CHECK(a == Rational<T>(12, 4));
    CHECK(not signbit(a));
    CHECK(abs(a) == a);
    }

    // Construction from zero
    {
    constexpr Rational<T> a(0);
    CHECK(a.is_valid() && a.is_zero());
    CHECK(is_rational_v<decltype(a)>);
    CHECK(a.p == 0 && a.q == 1);
    CHECK(static_cast<double>(a) == 0.);
    CHECK(a == Rational<T>(0, 4));
    CHECK(not signbit(a));
    CHECK(abs(a) == a);
    }

    // TODO: test simplify at construction

    // Construction from two integers
    {
    constexpr Rational<T> a(3, 2);
    CHECK(a.is_valid() && !a.is_zero());
    CHECK(is_rational_v<decltype(a)>);
    CHECK(a.p == 3 && a.q == 2);
    CHECK(static_cast<double>(a) == 1.5);
    CHECK(a == Rational<T>(-9, -6));
    CHECK(not signbit(a));
    CHECK(abs(a) == a);
    }

    // Construction of a negative rational
    {
    constexpr Rational<T> a(-3, 2);
    CHECK(a.is_valid() && !a.is_zero());
    CHECK(is_rational_v<decltype(a)>);
    CHECK(a.p == -3 && a.q == 2);
    CHECK(static_cast<double>(a) == -1.5);
    CHECK(a == Rational<T>(9, -6));
    CHECK(signbit(a));
    CHECK(abs(a) == Rational<T>(3, 2));
    }

    // Internal product of two rationals
    {
    constexpr Rational<T> a(3, 2);
    constexpr Rational<T> b(4, 3);
    constexpr auto ab = a * b;
    constexpr auto ba = b * a;

    CHECK(ab.p == 2 and ba.p == 2);
    CHECK(ab.q == 1 and ba.q == 1);
    }
     
    // Internal sum of two rationals
    {
    constexpr Rational<T> a(1, 2);
    constexpr Rational<T> b(3, 4);
    constexpr auto ab = a + b;
    constexpr auto ba = b + a;

    CHECK(ab.p == 5 and ba.p == 5);
    CHECK(ab.q == 4 and ba.q == 4);
    }

    // External product of a rational and an integer
    {
    constexpr Rational<T> a(3, 4);
    constexpr T b = 2;
    constexpr auto ab = a * b;
    constexpr auto ba = b * a;
    constexpr auto ab2 = 2 * ab;

    CHECK(ab.p == 3 and ba.p == 3);
    CHECK(ab.q == 2 and ba.q == 2);
    CHECK(ab2 == 3);
    }

    // External sum of a rational and an integer
    {
    constexpr Rational<T> a(2, 3);
    constexpr T b = 2;
    constexpr auto ab = a + b;
    constexpr auto ba = b + a;

    CHECK(ab.p == 8 and ba.p == 8);
    CHECK(ab.q == 3 and ba.q == 3);
    }

    // Comparison (TODO: constexpr)
    {
        auto check_helper = [] (auto a, auto b)
        {
            bool lt = a < b; CHECK(lt);
            bool ngt = not (a > b); CHECK(ngt);
            bool gt = b > a; CHECK(gt);
            bool nlt = not (b < a); CHECK(nlt);
        };

        check_helper(Rational<T>(1, 2), Rational<T>(2, 3));
        check_helper(Rational<T>(-2, 3), Rational<T>(1, 3));
        check_helper(Rational<T>(2, -3), Rational<T>(1, 3));
        check_helper(-1, Rational<T>(1, 2));
        check_helper(Rational<T>(1, 2), 2);
    }


    return return_code();
}
