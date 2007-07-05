EXTRA_DIST = do_all_tests.sh testinclude.hpp

noinst_LTLIBRARIES = libamethyst_test.la

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

libamethyst_test_la_LIBADD = @top_srcdir@/src/reference_material/general/libamethyst_general.la

noinst_PROGRAMS = test_raster test_rgbcolor test_fd_stream test_tokenizer test_tests
test_fd_stream_SOURCES = test_fd_stream.cpp
test_fd_stream_LDADD = @top_srcdir@/src/reference_material/general/libamethyst_general.la
test_tokenizer_SOURCES = test_tokenizer.cpp
test_tokenizer_LDADD = @top_srcdir@/src/reference_material/general/libamethyst_general.la
test_raster_SOURCES = test_raster.cpp
test_rgbcolor_SOURCES = test_rgbcolor.cpp
INCLUDES=-I@top_srcdir@/src/reference_material/general -I@top_srcdir@/src/reference_material/
# Turn off optimizations, so that these tests will actually execute.
AM_CXXFLAGS=-O0 -ggdb -Wall

TESTS = do_all_tests.sh

# This one tests some passing/failure in the amethyst test library.  Very
# incomplete and will probably be removed as more tests are written.
test_tests_SOURCES = test_tests.cpp
test_tests_LDADD = libamethyst_test.la

#TESTS = test_tests
