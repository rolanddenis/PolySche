set(EXAMPLES_FILES
    example_rational
)

foreach(FILE ${EXAMPLES_FILES})
  add_executable(${FILE} ${FILE}.cpp)
endforeach(FILE)
