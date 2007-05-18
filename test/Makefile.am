EXTRA_DIST = do_all_tests.sh testinclude.hpp
noinst_PROGRAMS = test_raster test_rgbcolor test_fd_stream test_tokenizer
test_fd_stream_SOURCES = test_fd_stream.cpp
test_fd_stream_LDADD = @top_srcdir@/src/reference_material/general/libamethyst_general.la
test_tokenizer_SOURCES = test_tokenizer.cpp
test_tokenizer_LDADD = @top_srcdir@/src/reference_material/general/libamethyst_general.la
test_raster_SOURCES = test_raster.cpp
test_rgbcolor_SOURCES = test_rgbcolor.cpp
INCLUDES=-I@top_srcdir@/src/reference_material/general -I@top_srcdir@/src/reference_material/
# Turn off optimizations, so that these tests will actually execute.
AM_CXXFLAGS=-O0 -ggdb

