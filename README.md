[![Project license](https://img.shields.io/github/license/rolanddenis/PolySche.svg?style=flat-square)](LICENSE)
![CI](https://github.com/rolanddenis/PolySche/actions/workflows/ci.yml/badge.svg)

The goal of this C++17 library is to statically (eventually at compile-time) **generate stencils** involved
in finite differences and finite volumes schemes that are based on **polynomial interpolation**.

An interpolation polynomial is generated by solving a set of **linear equations** defined by some operations
(evaluation, derivation, integration, ...) applied to a canonical polynomial.

Once generated, this **interpolation polynomial** can be used as it (general formulation of the interpolation)
or can be derivated and evaluated at a given point so that to recover the associated numerical scheme **stencil**.

The coefficients are computed in the **rational** space so that to avoid numerical precision issues (except integer overflow).

# Examples

## Finite difference scheme

### Value constraints

To build classical finite difference schemes, the first step is to build an interpolation polynomial
from a step of interpolation positions and values.

Commonly, we use evenly spaced discretization of the real line and we can use $dx = 1$
since general formulation is easily recovered using a simple scaling function (taking care of the derivation order).

For a 2nd-order approximation, the code looks like:
```C++
#include <polysche/polynomial_scheme.hpp>

...

using polysche::PolynomialScheme;
constexpr auto PS = PolynomialScheme<2>{}; // Order 2 interpolation
constexpr auto P = PS.get_polynomial(); // Gets the canonical polynomial used to express the constraints
constexpr auto S = PS.add_eqn(P(-1)) // constraint P(-1) = u_{-1} 
                     .add_eqn(P( 0)) // constraint P(0) = u_0
                     .add_eqn(P( 1)) // constraint P(1) = u_1
                     .solve();       // Solve the linear system
constexpr auto fd1 = S.derivate()(0); // Derivate the interpolation polynomial and evaluate it at 0
std::cout << fd1 << std::endl; // Supposing that << is overloaded for std::array
```
that should display `[-1/2,0,1/2]` if the `<<` operator is overloaded for `std::array`.

Each returned coefficient is associated to a second term of the contraints, in the same order, that is here:
$$u'(0) \approx -\frac12 u(-1) + \frac12 u(1).$$

The intermediate variable `S` actually contains the interpolation polynomial:
```C++
std::cout << "S = " << S << std::endl;
```
that displays
```C++
[0,1,0] + [1,0,0] X + [-1,-1,1] X^2
```
where, as above, each monomial is expressed as a linear combination of the second terms of the constraints :
$$S(X) := u(0) + u(-1) X + (-u(-1) - u(0) + u(1)) X^2.$$

We can then reuse this polynomial to compute another stencils, for example :
- for the approximation of the second-order derivative (centered):
    ```C++
    constexpr fd2 = S.derivate(2)(0);
    std::cout << fd2 << std::endl;
    ```
    that displays
    ```C++
    [1,-2,1]
    ```
- for a non-centered approximation of the first-order derivative, eg when at the domain boundaries :
    ```C++
    constexpr fd1_left = S.derivate()(-1);
    std::cout << fd1_left << std::endl;
    ```
    that displays
    ```C++
    [-3/2,2,-1/2]
    ```

### Mixing value and derivative constraints

We can also add constraint on the derivative of the interpolation polynomial, for example to include a boundary condition like a Neumann condition :
```C++
#include <polysche/polynomial_scheme.hpp>

...

using polysche::PolynomialScheme;
constexpr auto PS = PolynomialScheme<2>{}; // Order 2 interpolation
constexpr auto P = PS.get_polynomial(); // Gets the canonical polynomial used to express the constraints
constexpr auto S = PS.add_eqn(P.derivative()(0)) // constraint P'(0) = v_0
                     .add_eqn(P( 0)) // constraint P(0) = u_0
                     .add_eqn(P( 1)) // constraint P(1) = u_1
                     .solve();       // Solve the linear system
constexpr auto fd2_neumann = S.derivate(2)(0);
std::cout << fd2_neumann << std::endl;
```
that displays, for an approximation of the 2th derivative at $x = 0$:
```C++
[-2,-2,2]
```
where the first coefficient is associated to the wanted slope at $x = 0$ ($0$ for a Neumann condition).

It can also be used to extrapolate data outside the boundary to fill ghost cells in the so-called ghost-cell methods:
```C++
std::cout << S(-1) << std::endl;
```
that displays
```C++
[-2,0,1]
```
meaning that the ghost cell $u_{-1}$ should be filled with $-2 u'_0 + u_1$.


## Cubic Hermite spline

The same way, we can recover higher order interpolation like the cubic Hermite spline:
```C++
#include <polysche/polynomial_scheme.hpp>

...

using polysche::PolynomialScheme;
constexpr auto PS = PolynomialScheme<3>{}; // Interpolation of order 3
constexpr auto P = PS.get_polynomial();
constexpr auto S = PS
    .add_eqn(P(0)) // = f(0)
    .add_eqn(P(1)) // = f(1)
    .add_eqn(P.derivate()(0)) // = f'(0)
    .add_eqn(P.derivate()(1)) // = f'(1)
    .solve();
std::cout << "S = " << S << std::endl;
```
that displays:
```C++
S = [1,0,0,0] + [0,0,1,0] X + [-3,3,-2,-1] X^2 + [2,-2,1,1] X^3
```

## Finite volume method

We can also define constraint based on the integration of the polynomial between two bounds.
That way, we can construct interpolations adapted to finite volume formulation and use them,
for example, to interpolate solution on finer resolution grid, like the prediction step in
multi-resolution methods.

A first example with interpolation of order 2:
```C++
#include <polysche/polynomial_scheme.hpp>

...

using polysche::PolynomialScheme;
constexpr auto PS = PolynomialScheme<2>{};
constexpr auto P = PS.get_polynomial();
constexpr auto S = PS.add_eqn(P.integrate({-3, 2}, {-1, 2})) // = u_{-1}
                     .add_eqn(P.integrate({-1, 2}, { 1, 2})) // = u_0
                     .add_eqn(P.integrate({ 1, 2}, { 3, 2})) // = u_1
                     .solve();
constexpr auto left  = S.integrate({-1, 2}, 0);
constexpr auto right = S.integrate(0, {1, 2});
std::cout << "left(u_0)  = " << left << std::endl;
std::cout << "right(u_0) = " << right << std::endl;
```
that displays:
```C++
left(u_0)  = [1/16,1/2,-1/16]
right(u_0) = [-1/16,1/2,1/16]
```
that is the expected stencil relatively to a factor 2 (integration should be scaled by the space step).

We can also generalize it to higher order interpolation:
```C++
#include <polysche/polynomial_scheme.hpp>

using polysche::PolynomialScheme;

template <std::size_t Order>
constexpr auto make_finite_volume() noexcept
{
    auto PS = PolynomialScheme<Order>{};
    auto P = PS.get_polynomial();
    for (int i = - static_cast<int>(Order) / 2; i <= static_cast<int>(Order) / 2; ++i)
        PS = PS.add_eqn(P.integrate({2 * i - 1, 2}, {2 * i + 1, 2}));
    return PS.solve();
}

...
std::cout << "Finite volume of order 8:" << std::endl;
constexpr auto S = make_finite_volume<8>();
std::cout << "int_{-1/2}^0 S(x) = " << S.integrate({-1, 2}, 0) << std::endl;
std::cout << "int_0^{1/2}  S(x) = " << S.integrate(0, {1, 2})  << std::endl;
```
that displays:
```C++
Finite volume of order 8:
int_{-1/2}^0 S(x) = [-35/65536,185/32768,-949/32768,3461/32768,1/2,-3461/32768,949/32768,-185/32768,35/65536]
int_0^{1/2}  S(x) = [35/65536,-185/32768,949/32768,-3461/32768,1/2,3461/32768,-949/32768,185/32768,-35/65536]
```

And we can also mix with custom boundary conditions, here with **Neumann condition**:
```C++
#include <polysche/polynomial_scheme.hpp>

...

using polysche::PolynomialScheme;
std::cout << "Finite volume of order 2 with Neumann condition:" << std::endl;
constexpr auto PS = PolynomialScheme<2>{};
constexpr auto P = PS.get_polynomial();
constexpr auto S = PS
    .add_eqn(P.integrate({-1, 2}, { 1, 2})) // \int_{-1/2}^{1/2} u = u_0
    .add_eqn(P.integrate({ 1, 2}, { 3, 2})) // \int_{1/2}^{3/2} u = u_1
    .add_eqn(P.derivate()(Rational(3, 2)))  // u'(3/2) = c
    .solve();
std::cout << "S(X) = " << S << std::endl;
std::cout << "left(u_1)  = " << S.integrate({1, 2}, 1) << std::endl;
std::cout << "right(u_1) = " << S.integrate(1, {3, 2})  << std::endl; 
```
that displays:
```C++
Finite volume of order 2 with Neumann condition:
S(X) = [23/24,1/24,-1/24] + [-3/2,3/2,-1/2] X + [1/2,-1/2,1/2] X^2
left(u_1)  = [1/16,7/16,-1/16]
right(u_1) = [-1/16,9/16,1/16]
```
and also fill ghost values:
```C++
std::cout << "u_2 = " << S.integrate({3, 2}, {5, 2}) << std::endl;
std::cout << "u_3 = " << S.integrate({5, 2}, {7, 2}) << std::endl;
```
that displays:
```C++
u_2 = [0,1,1]
u_3 = [1,0,3]
```

Same with **Dirichlet condition**:
```C++
#include <polysche/polynomial_scheme.hpp>

...

using polysche::PolynomialScheme;
std::cout << "Finite volume of order 2 with Dirichlet condition:" << std::endl;
constexpr auto PS = PolynomialScheme<2>{};
constexpr auto P = PS.get_polynomial();
constexpr auto S = PS
    .add_eqn(P.integrate({-1, 2}, { 1, 2})) // \int_{-1/2}^{1/2} u = u_0
    .add_eqn(P.integrate({ 1, 2}, { 3, 2})) // \int_{1/2}^{3/2} u = u_1
    .add_eqn(P(Rational(3, 2)))             // u(3/2) = c
    .solve();
std::cout << "S(X) = " << S << std::endl;
std::cout << "left(u_1)  = " << S.integrate({1, 2}, 1) << std::endl;
std::cout << "right(u_1) = " << S.integrate(1, {3, 2})  << std::endl;   
std::cout << "u_2 = " << S.integrate({3, 2}, {5, 2}) << std::endl;
std::cout << "u_3 = " << S.integrate({5, 2}, {7, 2}) << std::endl;
std::cout << std::endl;
```
that displays:
```C++
Finite volume of order 2 with Dirichlet condition:
S(X) = [15/16,3/16,-1/8] + [-7/4,13/4,-3/2] X + [3/4,-9/4,3/2] X^2
left(u_1)  = [1/32,21/32,-3/16]
right(u_1) = [-1/32,11/32,3/16]
u_2 = [1/2,-5/2,3]
u_3 = [5/2,-21/2,9]
```


