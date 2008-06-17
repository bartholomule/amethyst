EXTRA_DIST = do_all_tests.sh testinclude.hpp

SUBDIRS = test_framework

# Turn off optimizations, so that these tests will actually execute.
AM_CXXFLAGS = -Wall


BUILT_TESTS = \
	test_quaternion \
	test_disc

TESTS = \
	do_all_tests.sh \
	$(BUILT_TESTS)


LDADD = \
	-Ltest_framework -lamethyst_test \
	-L$(top_builddir)/src/reference_material/general -lamethyst_general \
	-L$(top_builddir)/src/reference_material/graphics -lamethyst_graphics

INCLUDES = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/test

noinst_PROGRAMS = test_raster test_rgbcolor test_fd_stream test_tokenizer $(BUILT_TESTS)

test_fd_stream_SOURCES = test_fd_stream.cpp
test_tokenizer_SOURCES = test_tokenizer.cpp
test_raster_SOURCES = test_raster.cpp
test_rgbcolor_SOURCES = test_rgbcolor.cpp
test_quaternion_SOURCES = test_quaternion.cpp
test_disc_SOURCES = test_disc.cpp

