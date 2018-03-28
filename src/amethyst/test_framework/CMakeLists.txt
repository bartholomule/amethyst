add_library(amethyst_test
	testinclude.hpp
	unit_test_fwd.hpp
	unit_test.hpp
	unit_test.cpp
	unit_test_aggregator.hpp
	unit_test_aggregator.cpp
	unit_test_exceptions.hpp
	unit_test_exceptions.cpp
	unit_test_info.hpp
	unit_test_info.cpp
	unit_test_auto.hpp
	unit_test_auto.cpp
)

add_executable(test_tests test_tests.cpp)
target_link_libraries(test_tests amethyst_test)
#add_test(NAME test_tests COMMAND $<TARGET_FILE:test_tests>)

