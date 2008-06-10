EXTRA_DIST = do_all_tests.sh testinclude.hpp

SUBDIRS = test_framework

TESTS = test_quaternion do_all_tests.sh

# Turn off optimizations, so that these tests will actually execute.
AM_CXXFLAGS = -O0 -ggdb -Wall

LDADD = \
	-Ltest_framework -lamethyst_test \
	$(top_srcdir)/src/reference_material/general/libamethyst_general.la

INCLUDES = \
	-I$(top_srcdir)/src/reference_material/general \
	-I$(top_srcdir)/src/reference_material/ \
	-I$(top_srcdir)/test/test_framework

noinst_PROGRAMS = test_raster test_rgbcolor test_fd_stream test_tokenizer test_quaternion

test_fd_stream_SOURCES = test_fd_stream.cpp
test_tokenizer_SOURCES = test_tokenizer.cpp
test_raster_SOURCES = test_raster.cpp
test_rgbcolor_SOURCES = test_rgbcolor.cpp
test_quaternion_SOURCES = test_quaternion.cpp

