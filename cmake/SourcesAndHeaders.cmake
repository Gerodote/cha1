set(sources
  src/generator_polynomial.cpp
)

set(exe_sources
		src/main.cpp
		${sources}
)

set(headers
    include/data/generator_polynomial.hpp
    include/extra/vandermonde.hpp
)

set(test_sources
  src/tmp_test.cpp
)
