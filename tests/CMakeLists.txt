set(TESTS_FILES
    test_rational
    test_gauss
    test_polynomial
    test_polynomial_scheme
)

foreach(FILE ${TESTS_FILES})
  add_executable(${FILE} ${FILE}.cpp)
  add_test(${FILE} ${FILE})
endforeach(FILE)
