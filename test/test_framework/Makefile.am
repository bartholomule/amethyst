noinst_LTLIBRARIES = libamethyst_test.la

# Turn off optimizations, so that these tests will actually execute.
AM_CXXFLAGS=-O0 -ggdb -Wall

INCLUDES=-I$(top_srcdir)/src/reference_material/general -I$(top_srcdir)/src/reference_material/

# TESTS = test_tests

noinst_PROGRAMS = test_tests

libamethyst_test_la_SOURCES = \
testinclude.hpp \
unit_test_fwd.hpp \
unit_test.hpp \
unit_test.cpp \
unit_test_aggregator.hpp \
unit_test_aggregator.cpp \
unit_test_exceptions.hpp \
unit_test_exceptions.cpp \
unit_test_info.hpp \
unit_test_info.cpp \
unit_test_auto.hpp \
unit_test_auto.cpp


# This one tests some passing/failure in the amethyst test library.  Very
# incomplete and will probably be removed as more tests are written.
test_tests_SOURCES = test_tests.cpp
test_tests_LDADD = libamethyst_test.la

