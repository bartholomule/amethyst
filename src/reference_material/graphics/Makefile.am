lib_LTLIBRARIES = libamethyst_graphics.la
libamethyst_graphics_la_SOURCES = \
	capabilities.cpp \
	image_converter.cpp \
	requirements.cpp

INCLUDES = -I$(top_srcdir)/include

libamethyst_graphics_la_LIBADD = -L$(top_builddir)/src/reference_material/general -lamethyst_general
libamethyst_graphics_la_LDFLAGS = -shared -rpath @prefix@/lib -version-info 1

