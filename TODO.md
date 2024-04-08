
# TODO

- proper tests & CI
- arithmetic on stencil and polynomial to express more stuff (scaling, Robin conditions, etc.),
- batch of equations (same polynomial evaluated at multiple positions),
- forcing usage of rational numbers with same integer type (to avoid promoted type and unwanted conversion when solving the linear system).
- casting function from stencil to `std::array` (or other) of custom type.
- a Rational multiplied by a float should behave like if cast to float of the rational was requested.
- sparse polynomial? 
- Rational should be simplified at construction so that there is not need to calling `simplify` at each operation becomes unnecessary.
- detecting possible overflows.
- checking linear system inversion.