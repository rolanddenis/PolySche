#include <iostream>

#include "polynomial_scheme.hpp"
#include "utils.hpp"
#include "rational.hpp"

template <std::size_t Order>
constexpr auto make_finite_volume() noexcept
{
    auto PS = PolynomialScheme<Order>{};
    auto P = PS.get_polynomial();
    for (int i = - static_cast<int>(Order) / 2; i <= static_cast<int>(Order) / 2; ++i)
        PS = PS.add_eqn(P.integrate({2 * i - 1, 2}, {2 * i + 1, 2}));
    return PS.solve();
}


int main()
{
    {
    std::cout << "Finite differences of order 2:" << std::endl;
    constexpr auto PS = PolynomialScheme<2>{}; // Order 2
    constexpr auto P = PS.get_polynomial();
    constexpr auto S = PS.add_eqn(P(-1)) // u_{-1}
                        .add_eqn(P( 0)) // u_0
                        .add_eqn(P( 1)) // u_1
                        .solve();
    std::cout << S << std::endl;
    std::cout << S.derivate(1)(0) << std::endl;
    std::cout << S.derivate(2)(0) << std::endl;
    std::cout << S.derivate(1)(-1) << std::endl;
    std::cout << S(-2) << std::endl;
    std::cout << std::endl;
    }

    {
    std::cout << "Finite differences of order 2 with Neumann boundary condition:" << std::endl;
    constexpr auto PS = PolynomialScheme<2>{}; // Order 2
    constexpr auto P = PS.get_polynomial();
    constexpr auto S = PS.add_eqn(P.derivate()(0)) // u'_0
                        .add_eqn(P( 0)) // u_0
                        .add_eqn(P( 1)) // u_1
                        .solve();
    std::cout << S << std::endl;
    std::cout << S.derivate(1)(0) << std::endl;
    std::cout << S.derivate(2)(0) << std::endl;
    std::cout << S(-1) << std::endl;
    std::cout << std::endl;
    }

    {
    std::cout << "Cubic Hermite spline:" << std::endl;
    constexpr auto PS = PolynomialScheme<3>{};
    constexpr auto P = PS.get_polynomial();
    constexpr auto S = PS
        .add_eqn(P(0)) // = f(0)
        .add_eqn(P(1)) // = f(1)
        .add_eqn(P.derivate()(0)) // = f'(0)
        .add_eqn(P.derivate()(1)) // = f'(1)
        .solve();
    std::cout << "S = " << S << std::endl;
    std::cout << std::endl;
    }

    {
    std::cout << "Finite volumes of order 2:" << std::endl;
    constexpr auto PS = PolynomialScheme<2>{};
    constexpr auto P = PS.get_polynomial();
    constexpr auto S = PS.add_eqn(P.integrate({-3, 2}, {-1, 2})) // u_{-1}
                        .add_eqn(P.integrate({-1, 2}, { 1, 2})) // u_0
                        .add_eqn(P.integrate({ 1, 2}, { 3, 2})) // u_1
                        .solve();
    std::cout << S << std::endl;
    std::cout << "left(u_0) = " << S.integrate({-1, 2}, 0) << std::endl; // {1/8, 1, -1/8} for prediction in multi-resolution
    std::cout << "right(u_0) = " << S.integrate(0, {1, 2}) << std::endl; // {-1/8, 1, 1/8} for prediction in multi-resolution
    std::cout << S.derivate()(Rational{-1, 2}) << std::endl;
    std::cout << S.derivate()(0) << std::endl;
    std::cout << std::endl;
   }

    {
    std::cout << "Finite volumes of order 4:" << std::endl;
    constexpr auto PS = PolynomialScheme<4>{};
    constexpr auto P = PS.get_polynomial();
    std::cout << "P = " << P << std::endl;
    constexpr auto S = PS
        .add_eqn(P.integrate({-5, 2}, {-3, 2})) // = u_{-2}
        .add_eqn(P.integrate({-3, 2}, {-1, 2})) // = u_{-1}
        .add_eqn(P.integrate({-1, 2}, { 1, 2})) // = u_0
        .add_eqn(P.integrate({ 1, 2}, { 3, 2})) // = u_1
        .add_eqn(P.integrate({ 3, 2}, { 5, 2})) // = u_2
        .solve();
    std::cout << "S = " << S << std::endl;
    std::cout << "int_{-1/2}^0 S = " << S.integrate({-1, 2}, 0) << std::endl;
    std::cout << "int_0^{1/2} S = " << S.integrate(0, {1, 2}) << std::endl;
    std::cout << "S' = " << S.derivate() << std::endl;
    std::cout << S.derivate()(Rational(-1, 2)) << std::endl;
    std::cout << S.derivate()(-1) << std::endl;
    std::cout << std::endl;
    }

    {
    std::cout << "Finite volume of order 6:" << std::endl;
    constexpr auto S = make_finite_volume<6>();
    std::cout << "int_{-1/2}^0 S = " << S.integrate({-1, 2}, 0) << std::endl;
    std::cout << "int_0^{1/2} S = " << S.integrate(0, {1, 2}) << std::endl;
    std::cout << std::endl;
    }

    {
    std::cout << "Finite volume of order 8:" << std::endl;
    constexpr auto S = make_finite_volume<8>();
    std::cout << "int_{-1/2}^0 S(x) = " << S.integrate({-1, 2}, 0) << std::endl;
    std::cout << "int_0^{1/2}  S(x) = " << S.integrate(0, {1, 2})  << std::endl;
    std::cout << std::endl;
    }

    {
    std::cout << "Finite volume of order 10:" << std::endl;
    constexpr auto S = make_finite_volume<10>();
    std::cout << "int_{-1/2}^0 S = " << S.integrate({-1, 2}, 0) << std::endl;
    std::cout << "int_0^{1/2} S = " << S.integrate(0, {1, 2}) << std::endl;
    std::cout << std::endl;
    }

    {
    std::cout << "Finite volumes of order 2 with Neumann condition:" << std::endl;
    constexpr auto PS = PolynomialScheme<2>{};
    constexpr auto P = PS.get_polynomial();
    constexpr auto S = PS.add_eqn(P.derivate()(Rational(-1, 2))) // = u'(-1/2)
                         .add_eqn(P.integrate({-1, 2}, { 1, 2})) // u_0
                         .add_eqn(P.integrate({ 1, 2}, { 3, 2})) // u_1
                         .solve();
    std::cout << "S(X) = " << S << std::endl;
    std::cout << "left(u_0)  = " << S.integrate({-1, 2}, 0) << std::endl;
    std::cout << "right(u_0) = " << S.integrate(0, {1, 2})  << std::endl;   
    }

    return 0;
}

