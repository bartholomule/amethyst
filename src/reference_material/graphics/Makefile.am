lib_LTLIBRARIES = libamethyst_graphics.la
libamethyst_graphics_la_SOURCES = \
	capabilities.cpp \
	image_converter.cpp \
	requirements.cpp

pkginclude_HEADERS = \
	capabilities.hpp \
	requirements.hpp \
	conditional_value.hpp \
	interpolated_value.hpp \
	rgbcolor.hpp \
	\
	shape.hpp \
	aggregate.hpp \
	plane.hpp \
	disc.hpp \
	rectangle.hpp \
	triangle.hpp \
	sphere.hpp \
	dynamic_sphere.hpp \
	\
	texture.hpp \
	simple_texture.hpp \
	surface_texture.hpp \
	solid_texture.hpp \
	noise_texture.hpp \
	marble_texture.hpp \
	\
	material.hpp \
	\
	raster.hpp \
	image.hpp \
	image_converter.hpp \
	\
	intersection_info.hpp \
	ray_parameters.hpp \
	\
	base_camera.hpp \
	pinhole_camera.hpp \
	\
	samplegen1d.hpp \
	samplegen2d.hpp



INCLUDES = -I$(top_srcdir)/include

libamethyst_graphics_la_LIBADD = -L$(top_builddir)/src/reference_material/general -lamethyst_general
libamethyst_graphics_la_LDFLAGS = -shared -rpath @prefix@/lib -version-info 1

